#ifndef FAT16_H
#define FAT16_H

#include "../STD/bool.h"
#include "../STD/int.h"
#include "../STD/defs.h"

#define INVALID_LBA UINT32_MAX



#define FAT_EOC_START 0xFFF8
#define FAT_BAD_SECTOR 0xFFF7
#define FAT_FREE_SECTOR 0x0000

#define MAX_DIR_ENTRIES 256
#define MAX_NAME_LEN   256   /* enough for LFN */

// Max LFN length (255 chars + '\0')
#define MAX_LFN 256






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



typedef enum fat16_error_type
{
    OK,
    BOOT_SECTOR_READ_FAIL,
    INVALID_BOOT_SIGNATURE,
    INVALID_EBR_SIGNATURE,
    CRITICAL_BPB_VALUE_IS_ZERO,
    FAT_LOAD_FAIL,
    ROOT_DIRECTORY_LOAD_FAIL,



} fat16_error_type;

fat16_error_type get_fat16_last_error();
void set_fat16_last_error(fat16_error_type e);
int fat16_ok(void);
int fat16_fail(fat16_error_type e);

uint32_t cluster_to_lba(Fat16 *fat, uint16_t cluster);
uint32_t bytes_to_sectors(Fat16 *fat, uint32_t bytes);
uint16_t sectors_to_clusters(Fat16 *fat, uint32_t sectors);
bool read_cluster(Fat16 *fat, uint16_t cluster, uint8_t *dest);
uint8_t fat_init(Fat16 *fat);
bool read_boot_sector(Fat16 *fat);
bool load_fat(Fat16 *fat);
bool load_root_directory(Fat16 *fat);
DirEntry *find_file(Fat16 *fat, const char* name);
ClusterChain *traverse_chain(Fat16 *fat, uint16_t chain_start);
void *load_cluster_chain(Fat16 *fat, ClusterChain *chain, void *destination);
DirEntry *load_dir(Fat16 *fat, DirEntry *directory);
DirEntry *find_file_in_directory(Fat16 *fat, DirEntry *directory, const char* name);
parsed_dir *parse_dir(Fat16 *fat, DirEntry *dir, uint32_t max_entries);
char *parse_LFN_buffer(LFN_buffer *buffer);
bool parse_LFN_entry(const LFN_entry *lfn, LFN_buffer *buf);
void free_parsed_dir(parsed_dir *pd);
parsed_dir_entry *find_parsed_dir_entry(parsed_dir *dir, const char *name);

void *load_file(Fat16 *fat, DirEntry *file);

void *open(Fat16 *fat, const char *name);
bool __reload_open(Fat16 *fat, bool *dir_owned, DirEntry **current_dir, parsed_dir **current_parsed_dir, parsed_dir_entry *e);
uint32_t round_bytes_to_clusters(Fat16 *fat, uint32_t bytes);
#endif
