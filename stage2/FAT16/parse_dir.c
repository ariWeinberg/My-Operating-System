#include "fat16.h"
parsed_dir *parse_dir(Fat16 *fat, DirEntry *dir, uint32_t max_entries)
{
    parsed_dir *pd = zalloc(sizeof(parsed_dir));
    if (!pd)
    {
        fat16_fail(FAT16_MEMORY_ALLOCATION_FAIL);
        return NULL;
    }

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
        parsed_dir_entry *new_entries = realloc(pd->entries, (pd->count + 1) * sizeof(parsed_dir_entry));

        if (!new_entries)
        {
            free(p.name);
            fat16_fail(FAT16_MEMORY_ALLOCATION_FAIL);
            return NULL;
        }

        pd->entries = new_entries;
        pd->entries[pd->count++] = p;
    }
    fat16_ok();
    return pd;
}

void free_parsed_dir(parsed_dir *pd)
{
    if (!pd) return;

    for (uint32_t i = 0; i < pd->count; i++)
        free(pd->entries[i].name);

    free(pd->entries);
    free(pd);
    fat16_ok();
}



parsed_dir_entry *find_parsed_dir_entry(parsed_dir *dir, const char *name)
{
    for (int i = 0; i < dir->count; i++)
    {
        if (strcmp(dir->entries[i].name, name) == 0)
        {
            fat16_ok();
            return &dir->entries[i];
        }
    }
    fat16_fail(FAT16_ENTRY_NOT_FOUND);
    return NULL;
}
