// ===== FILE: fat16.c =====
// Purpose: Implements the core FAT16 filesystem operations declared in fat16.h.
//
// Functions (Defined):
//   fat_init, read_boot_sector, load_fat, load_root_directory
//   find_file, read_cluster
//   traverse_chain, load_cluster_chain, load_dir, find_file_in_directory
//
// Dependencies: fat16.h, ata_driver.h, memory_managment.h
// Notes: Handles boot sector reading, FAT table loading, root directory parsing, cluster chain traversal, 
//        and file/directory reading. Integrates helpers and on-disk structures.
// TODOs:
//   - Implement LFN support
//   - Handle . and .. directories
//   - Implement FAT mirroring and loop detection
//   - Implement validation: boot signature, FAT signature, FAT type
//   - Improve error handling for I/O operations and corrupted FAT entries
#include "fat16.h"
#include "../ATA_DRIVER/ATA.h"
#include "../MEMORY_MANAGMENT/memory_managment.h"
#include "../UTILS/utils.h"
#include "../STRING/string.h"
uint8_t fat_init(Fat16 *fat)
{
    if (read_boot_sector(fat))
    {
        return 1;
    }
    if (fat->bs.boot_signature != 0xAA55)
    {
        return 2;
    }
    if ((fat->bs.ebr.signature != 0x28) && (fat->bs.ebr.signature != 0x29))
    {
        // NOTE: Some FAT variants may not set EBR signature (relaxed later)
        return 3;
    }
    if (fat->bs.bpb.Bytes_per_sector == 0 || fat->bs.bpb.sectors_per_cluster == 0 || fat->bs.bpb.sectors_per_FAT == 0)
    {
        return 4;
    }
    
    fat->bytes_per_sector = fat->bs.bpb.Bytes_per_sector;
    fat->sectors_per_cluster = fat->bs.bpb.sectors_per_cluster;

    fat->fat_sectors = fat->bs.bpb.sectors_per_FAT;
    fat->fat_bytes = fat->fat_sectors * fat->bytes_per_sector;
    fat->fat_start_lba = fat->bs.bpb.reserved_sectors;

    fat->root_dir_start_lba = fat->bs.bpb.reserved_sectors + (fat->bs.bpb.FAT_count * fat->bs.bpb.sectors_per_FAT);
    fat->root_dir_bytes = fat->bs.bpb.root_dir_entries * sizeof(DirEntry);
    fat->root_dir_sectors = fat->root_dir_bytes / fat->bytes_per_sector;
    if(fat->root_dir_bytes % fat->bytes_per_sector)
    {fat->root_dir_sectors ++;}
    
    fat->data_start_lba =
    fat->root_dir_start_lba + fat->root_dir_sectors;

    
    if(load_fat(fat))
    {
        free(fat->fat);
        return 5;
    }
    if(load_root_directory(fat))
    {
        free(fat->fat);
        free(fat->root_dir);
        return 5;
    }

    return 0;
}

bool read_boot_sector(Fat16 *fat)
{
    pio28_read(&(fat->bs), &ata_primary, 1, 0);
    return false;
}

bool load_fat(Fat16 *fat)
{
    
    fat->fat = zalloc(fat->fat_sectors * fat->bytes_per_sector);
    
    // Fat16 entries are little-endian; code assumes LE CPU
    pio28_read(fat->fat, &ata_primary, fat->fat_sectors, fat->fat_start_lba);
    return false;
}

bool load_root_directory(Fat16 *fat)
{
    fat->root_dir = zalloc(fat->root_dir_sectors * fat->bytes_per_sector);
    pio28_read(fat->root_dir, &ata_primary, fat->root_dir_sectors, fat->root_dir_start_lba);
    return false;
}


ClusterChain *traverse_chain(Fat16 *fat, uint16_t chain_start)
{   
    int i = 0;
    uint16_t current_cluster = chain_start;

    ClusterChain *chain = zalloc(sizeof(ClusterChain));
    chain->cluster_count = 0;
    chain->chain = NULL;

    while (current_cluster < FAT_EOC_START && chain->cluster_count < (fat->fat_bytes / 2))
    {
        if (current_cluster == FAT_BAD_SECTOR || current_cluster == FAT_FREE_SECTOR)
        {
            free(chain->chain);
            free(chain);
            return NULL;
        }
        // 
        // TODO: handle loops, and other FAT corruption problems....
        // 
        
        uint32_t fat_entries = fat->fat_bytes / 2;
        if (current_cluster >= fat_entries) {
            free(chain->chain);
            free(chain);
            return NULL;
        }
        
        chain->cluster_count++;
        chain->chain = realloc(chain->chain, chain->cluster_count * sizeof(uint16_t));
        if(!chain->chain)
        {
            free(chain->chain);
            free(chain);
            return NULL;
        }
        chain->chain[i] = current_cluster;
        current_cluster = fat->fat[current_cluster];
        
        i++;
    }
    if (chain->cluster_count == 0) {
        free(chain);
        return NULL;
    }        
    return chain;
}

void *load_cluster_chain(Fat16 *fat, ClusterChain *chain, void *destination)
{
    // 
    // TODO: return sectors read
    // 
    for (int i = 0; i < chain->cluster_count; i++)
    {
        uint8_t *ptr = (uint8_t *)destination;
        ptr += i * fat->sectors_per_cluster * fat->bytes_per_sector;

        read_cluster(fat, chain->chain[i], ptr);
        
    }
    return destination;
}

DirEntry *load_dir(Fat16 *fat, DirEntry *directory)
{
    ClusterChain *chain =
        traverse_chain(fat, directory->first_cluster_low);

    if (!chain)
        return NULL;

    uint32_t total_bytes =
        chain->cluster_count *
        fat->sectors_per_cluster *
        fat->bytes_per_sector;

    DirEntry *dest = zalloc(total_bytes);
    if (!dest)
    {
        free(chain->chain);
        free(chain);
        return NULL;
    }

    load_cluster_chain(fat, chain, dest);

    free(chain->chain);
    free(chain);
    return dest;
}

/**
 * @brief Clear an LFN buffer.
 *
 * Resets the internal LFN entry count.
 *
 * @param buf Pointer to LFN_buffer structure.
 */
static inline void lfn_buffer_clear(LFN_buffer *buf)
{
    buf->count = 0;
}


bool parse_LFN_entry(const LFN_entry *lfn, LFN_buffer *buf)
{
    if (buf->count >= 20)
        return false;

    parsed_LFN_entry *dst = &buf->entries[buf->count];

    dst->order    = lfn->order & 0x1F;
    dst->checksum = lfn->checksum;

    uint16_t tmp[13];
    uint32_t idx = 0;

    for (int i = 0; i < 5; i++) tmp[idx++] = lfn->value_first[i];
    for (int i = 0; i < 6; i++) tmp[idx++] = lfn->value_second[i];
    for (int i = 0; i < 2; i++) tmp[idx++] = lfn->value_third[i];

    utf16_to_ascii(tmp, 13, dst->name);

    buf->count++;
    return true;
}


char *parse_LFN_buffer(LFN_buffer *buffer)
{
    if (buffer->count == 0)
        return NULL;

    // Compute total length
    uint32_t total_len = buffer->count * 13;
    char *name = zalloc(total_len + 1);
    if (!name)
        return NULL;

    uint32_t pos = 0;

    // LFN entries are stored in reverse order
    for (int i = buffer->count - 1; i >= 0; i--)
    {
        for (int j = 0; j < 13 && buffer->entries[i].name[j]; j++)
            name[pos++] = buffer->entries[i].name[j];
    }

    name[pos] = '\0';
    return name;
}

char *normalize_SFN(const char sfn[11])
{
    // Max length: 8 + 1 + 3 + '\0' = 13
    char *out = zalloc(13);
    if (!out)
        return NULL;

    int pos = 0;

    // ---- base name (bytes 0–7) ----
    for (int i = 0; i < 8; i++)
    {
        if (sfn[i] == ' ')
            break;
        out[pos++] = sfn[i];
    }

    // ---- extension (bytes 8–10) ----
    if (sfn[8] != ' ')
    {
        out[pos++] = '.';
        for (int i = 8; i < 11; i++)
        {
            if (sfn[i] == ' ')
                break;
            out[pos++] = sfn[i];
        }
    }

    out[pos] = '\0';
    return out;
}

parsed_dir *parse_dir(Fat16 *fat, DirEntry *dir, uint32_t max_entries)
{
    parsed_dir *pd = zalloc(sizeof(parsed_dir));
    if (!pd)
        return NULL;

    pd->entries = NULL;
    pd->count   = 0;

    LFN_buffer lfn_buf;
    lfn_buffer_clear(&lfn_buf);

    for (uint32_t i = 0; i < max_entries; i++)
    {
        uint8_t first = (uint8_t)dir[i].filename8_3[0];

        // End of directory
        if (first == 0x00)
            break;

        // Deleted entry
        if (first == 0xE5)
        {
            lfn_buffer_clear(&lfn_buf);
            continue;
        }

        // LFN entry
        if (dir[i].Attributes == 0x0F)
        {
            parse_LFN_entry((LFN_entry *)&dir[i], &lfn_buf);
            continue;
        }

        // ---- SFN entry ----

        parsed_dir_entry p;
        p.attributes = dir[i].Attributes;
        p.first_cluster = dir[i].first_cluster_low;
        p.raw_entry  = &dir[i];

        if(dir[i].Attributes & 0x10)
        {
            ClusterChain *chain = traverse_chain(fat, dir[i].first_cluster_low);
            p.file_size = chain->cluster_count * fat->bytes_per_sector * fat->sectors_per_cluster;
            free(chain->chain);
            free(chain);

        }
        else
        {
            p.file_size  = dir[i].file_size;
        }

        // Name resolution
        if (lfn_buf.count > 0)
        {
            p.name = parse_LFN_buffer(&lfn_buf);
        }
        else
        {
            // Allocate 8.3 name (11 chars + NULL)
            p.name = normalize_SFN(dir[i].filename8_3);
        }

        lfn_buffer_clear(&lfn_buf);

        // Append to parsed directory
        parsed_dir_entry *new_entries =
            realloc(pd->entries, (pd->count + 1) * sizeof(parsed_dir_entry));

        if (!new_entries)
        {
            free(p.name);
            break;
        }

        pd->entries = new_entries;
        pd->entries[pd->count++] = p;
    }
    static bool a = false;
    if (!a)
    {
    clear_screen();
    }
    a = true;
    // print_string("\n\r parsed dir:\n\r");

    // for (int i = 0; i < pd->count; i++)
    // {
    //     char buf[30];
    //     print_string(pd->entries[i].name);
    //     print_string(" : ");
    //     print_string(uint32_to_str(strlen(pd->entries[i].name),buf));
    //     print_string("\n\r");
    // }
    // print_string("\n\r");

    return pd;
}

void free_parsed_dir(parsed_dir *pd)
{
    if (!pd) return;

    for (uint32_t i = 0; i < pd->count; i++)
        free(pd->entries[i].name);

    free(pd->entries);
    free(pd);
}


parsed_dir_entry *find_parsed_dir_entry(parsed_dir *dir, const char *name)
{
    for (int i = 0; i < dir->count; i++)
    {
        print_string("comparing: ");
        print_string(dir->entries[i].name);
        print_string(" -to- ");
        print_string(name);
        print_string("\n\r");
        if (strcmp(dir->entries[i].name, name) == 0)
        {
            return &dir->entries[i];
        }
    }
    return NULL;
}

void *load_file(Fat16 *fat, DirEntry *file)
{
    ClusterChain *chain = traverse_chain(fat, file->first_cluster_low);

    if (!chain)
        return NULL;

    uint32_t total_bytes = chain->cluster_count * fat->sectors_per_cluster * fat->bytes_per_sector;
    DirEntry *dest = zalloc(total_bytes);
    if (!dest)
    {
        free(chain->chain);
        free(chain);
        return NULL;
    }

    load_cluster_chain(fat, chain, dest);

    free(chain->chain);
    free(chain);
    return dest;
}

void *open(Fat16 *fat, const char *name)
{
    clear_screen();
    void *file = NULL;
    char **name_segments = (char**)NULL;
    bool dir_owned = false;
    uint16_t segments_count = strsplit(name, '/', &name_segments);
    DirEntry *current_dir = fat->root_dir;
    parsed_dir *current_parsed_dir = parse_dir(fat, current_dir, fat->bs.bpb.root_dir_entries);



    print_string("name to open: ");
    print_string(name);
    print_string("\n\n\r");
    if (current_parsed_dir) // return NULL on parsing error.
    {
        for (uint16_t i = 0; i < segments_count; i++)
        {
            // print_string("currently parsing: ");
            // print_string(name_segments[i]);
            // print_string("\n\r");


            parsed_dir_entry *e = find_parsed_dir_entry(current_parsed_dir, name_segments[i]);
            if (!e) {break;}
            if((!(e->attributes & 0x10)) && i < (segments_count - 1))  {break;}
            
            // print_string("entry seems valid!\n\r");
            
            if (i == segments_count - 1 && !(e->attributes & 0x10))
            {
                file = load_file(fat, e->raw_entry);
                break;
            }
            // print_string("entry is not the file!\n\r");
            if(__reload_open(fat, &dir_owned, &current_dir, &current_parsed_dir, e))
            {
                break;
            }
        }   
    }
    if(dir_owned)
    {free(current_dir);}
    free(current_parsed_dir);
    for(uint16_t j=0;j<segments_count;j++)
    {
        free(name_segments[j]);
    }
    free(name_segments);
    return file;
}

bool __reload_open(Fat16 *fat, bool *dir_owned, DirEntry **current_dir, parsed_dir **current_parsed_dir, parsed_dir_entry *e)

{
    if(*dir_owned)
    {free(*current_dir);}
    *current_dir = load_dir(fat, e->raw_entry);
    *dir_owned = true;
    if (!*current_dir) {return true;}
    free(*current_parsed_dir);
    // *current_parsed_dir = parse_dir(*current_dir, e->file_size / sizeof(DirEntry));
    *current_parsed_dir = parse_dir(fat, *current_dir, round_bytes_to_clusters(fat, e->file_size) / sizeof(DirEntry));
    // *current_parsed_dir = parse_dir(*current_dir, chain_size_in_clusters * fat->sectors_per_cluster * fat->bytes_per_sector / sizeof(DirEntry));

    if(!*current_parsed_dir) {return true;}

    return false;
}

uint32_t round_bytes_to_clusters(Fat16 *fat, uint32_t bytes)
{
    return (sectors_to_clusters(fat, bytes_to_sectors(fat, bytes)) * fat->bytes_per_sector * fat->sectors_per_cluster);
}