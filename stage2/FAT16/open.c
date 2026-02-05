#include "fat16.h"
#include "../MEMORY_MANAGMENT/memory_managment.h"
#include "../UTILS/utils.h"
#include "../STRING/string.h"

void *open(Fat16 *fat, const char *name)
{
    void *file = NULL;
    char **name_segments = (char**)NULL;
    bool dir_owned = false;
    uint16_t segments_count = strsplit(name, '/', &name_segments);
    DirEntry *current_dir = fat->root_dir;
    parsed_dir *current_parsed_dir = parse_dir(fat, current_dir, fat->bs.bpb.root_dir_entries);


    if (current_parsed_dir) // return NULL on parsing error.
    {
        for (uint16_t i = 0; i < segments_count; i++)
        {
            parsed_dir_entry *e = find_parsed_dir_entry(current_parsed_dir, name_segments[i]);
            if (!e) {break; fat16_fail(FAT16_ENTRY_NOT_FOUND);}
            if((!(e->attributes & 0x10)) && i < (segments_count - 1))  {break; fat16_fail(FAT16_INVALID_ATTRIBUTE);}
            
            if (i == segments_count - 1 && !(e->attributes & 0x10))
            {
                file = load_clustered_entry(fat, e->raw_entry);
                if(!file)
                {
                    fat16_fail(FAT16_ENTRY_LOAD_FAIL);
                    break;
                }
                break;
            }
            
            if(__reload_open(fat, &dir_owned, &current_dir, &current_parsed_dir, e))
            {
                break;
                fat16_fail(FAT16_DIRECTORY_PARSING_FAIL);
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
    if(file)
    {fat16_ok();}
    return file;
}

bool __reload_open(Fat16 *fat, bool *dir_owned, DirEntry **current_dir, parsed_dir **current_parsed_dir, parsed_dir_entry *e)

{
    if(*dir_owned)
    {free(*current_dir);}
    *current_dir = load_clustered_entry(fat, e->raw_entry);
    *dir_owned = true;
    if (!*current_dir) {return true;}
    free(*current_parsed_dir);
    *current_parsed_dir = parse_dir(fat, *current_dir, round_bytes_to_clusters(fat, e->file_size) / sizeof(DirEntry));

    if(!*current_parsed_dir) {return true;}

    return false;
}