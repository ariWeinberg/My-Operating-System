// ===== FILE: fat16_ondisk.h =====
// Purpose: Defines on-disk FAT16 structures with exact packing for reading/writing to disk.
//
// Types (Defined):
//   BPB        - BIOS Parameter Block
//   EBR        - Extended Boot Record
//   boot_sector- Complete boot sector structure
//   DirEntry   - Standard 8.3 directory entry
//   LFN_entry  - Long filename entry
//
// Dependencies: fat16_defs.h
// Notes: Structures correspond exactly to bytes on disk; uses __attribute__((packed)).
// TODOs: Handle . and .. directory entries, implement volume label handling
#ifndef FAT16_ONDISK_H
#define FAT16_ONDISK_H

#include "fat16_defs.h"


typedef struct BPB
{
    uint8_t jump_nop[3];
    char OEM_Identifier[8];
    uint16_t Bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t FAT_count;
    uint16_t root_dir_entries;
    uint16_t total_sector_count;
    uint8_t media_descriptor;
    uint16_t sectors_per_FAT;
    uint16_t sectors_per_track;
    uint16_t head_count;
    uint32_t hidden_sector_count;
    uint32_t large_sector_count;
} __attribute__((packed)) BPB;


typedef struct EBR
{
    uint8_t drive_number;
    uint8_t flags;
    uint8_t signature;
    uint32_t volume_id;
    char volume_label[11];
    char system_identifier[8];
} __attribute__((packed)) EBR;

typedef struct boot_sector
{
    struct BPB bpb;
    struct EBR ebr;
    uint8_t rest[448];
    uint16_t boot_signature;
} __attribute__((packed)) boot_sector;

typedef struct DirEntry
{
    char filename8_3[11];
    uint8_t Attributes;
    uint8_t reserved;
    uint8_t creation_time_hundrdths;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t first_cluster_high;
    uint16_t last_modification_time;
    uint16_t last_modification_date;
    uint16_t first_cluster_low;
    uint32_t file_size;
} __attribute__((packed)) DirEntry;


typedef struct LFN_entry
{
    uint8_t order;
    uint16_t value_first[5];
    uint8_t Attributes;
    uint8_t long_entry_type;
    uint8_t checksum;
    uint16_t value_second[6];
    uint16_t reserved;
    uint16_t value_third[2];
} __attribute__((packed)) LFN_entry;
#endif