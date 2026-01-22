// ===== FILE: fat16_structures.h =====
// Purpose: Defines in-memory structures for FAT16 filesystem handling.
//
// Types (Defined):
//   dir_item      - in-memory representation of a directory entry
//   Fat16         - main FAT16 object with boot sector, FAT table, root dir, and buffers
//   ClusterChain  - representation of a chain of clusters for a file/directory
//
// Dependencies: fat16_ondisk.h
// Notes: Bridges on-disk structures with runtime memory representation
// TODOs: Support more advanced metadata checks and validation
#ifndef FAT16_STRUCTURES_H
#define FAT16_STRUCTURES_H

#include "fat16_ondisk.h"

// Parsed LFN entry: represents one long filename fragment (ASCII only)
typedef struct parsed_LFN_entry
{
    char name[14];        // 13 characters from LFN entry + '\0'
    uint8_t attributes;   // attributes copied from the corresponding DirEntry or LFN entry
    uint8_t checksum;     // checksum for verification (from SFN)
    uint8_t order;
} parsed_LFN_entry;

// Temporary LFN buffer (cleared per directory entry)
typedef struct LFN_buffer
{
    parsed_LFN_entry entries[20]; // FAT16 max = 20 LFN entries
    uint8_t count;              // number of valid entries
} LFN_buffer;

typedef struct ClusterChain
{
    uint16_t cluster_count;
    uint16_t *chain;
} ClusterChain;

// Minimal representation of a parsed directory entry
typedef struct parsed_dir_entry
{
    char     *name;           // NULL-terminated, heap allocated
    uint8_t   attributes;
    uint16_t  first_cluster;
    uint32_t  file_size;

    DirEntry *raw_entry;      // pointer into original directory buffer
} parsed_dir_entry;

// Container for a parsed directory
typedef struct parsed_dir
{
    parsed_dir_entry *entries;   // dynamically allocated array
    size_t count;             // number of valid entries
} parsed_dir;

typedef struct Fat16
{
    boot_sector bs;

    uint32_t fat_start_lba;
    uint32_t fat_sectors;
    uint32_t fat_bytes;


    uint32_t root_dir_start_lba;
    uint16_t root_dir_sectors;
    uint32_t root_dir_bytes;

    uint32_t data_start_lba;
    
    uint16_t sectors_per_cluster;
    uint16_t bytes_per_sector;

    // Fat16 entries are little-endian; code assumes LE CPU
    uint16_t *fat;
    DirEntry *root_dir;
    
    
} Fat16;

#endif