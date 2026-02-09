#include "fat16.h"
#include "../MEMORY_MANAGMENT/memory_managment.h"
#include "../STRING/string.h"

/**
 * Get the size of a file in bytes.
 *
 * @param fat Pointer to initialized Fat16 structure
 * @param name Full path of the file
 * @return File size in bytes, or UINT32_MAX on error
 */
uint32_t get_file_size(Fat16 *fat, const char *name)
{
    if (!fat || !name) {
        fat16_fail(FAT16_INVALID_ARGUMENT);
        return UINT32_MAX;
    }

    // Parse the path and locate the file entry
    void *file_data = open(fat, name);
    if (!file_data) {
        return UINT32_MAX; // error already set by open()
    }

    // Find the parsed directory entry
    char **segments = NULL;
    uint16_t segments_count = strsplit(name, '/', &segments);
    if (segments_count == 0) {
        return UINT32_MAX;
    }

    DirEntry *current_dir = fat->root_dir;
    parsed_dir *current_parsed_dir = parse_dir(fat, current_dir, fat->bs.bpb.root_dir_entries);
    if (!current_parsed_dir) {
        return UINT32_MAX;
    }

    parsed_dir_entry *entry = NULL;
    for (uint16_t i = 0; i < segments_count; i++) {
        entry = find_parsed_dir_entry(current_parsed_dir, segments[i]);
        if (!entry) {
            free_parsed_dir(current_parsed_dir);
            return UINT32_MAX;
        }

        // If not last segment, descend into directory
        if (i < segments_count - 1) {
            parsed_dir *next_dir = parse_dir(fat, load_clustered_entry(fat, entry->raw_entry),
                                             round_bytes_to_clusters(fat, entry->file_size) / sizeof(DirEntry));
            free_parsed_dir(current_parsed_dir);
            current_parsed_dir = next_dir;
            if (!current_parsed_dir) {
                return UINT32_MAX;
            }
        }
    }

    uint32_t size = entry->file_size;

    // Clean up
    free_parsed_dir(current_parsed_dir);
    for (uint16_t j = 0; j < segments_count; j++) free(segments[j]);
    free(segments);
    free(file_data);

    fat16_ok();
    return size;
}
