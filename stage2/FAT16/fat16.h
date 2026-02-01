// ===== FILE: fat16.h =====
// Purpose: Public FAT16 interface, declares main functions for external use.
//
// Functions (Declared):
//   fat_init(Fat16 *fat)
//   read_boot_sector(Fat16 *fat)
//   load_fat(Fat16 *fat)
//   load_root_directory(Fat16 *fat)
//   find_file(Fat16 *fat, const char* name)
//   traverse_chain(Fat16 *fat, uint16_t chain_start)
//   load_cluster_chain(Fat16 *fat, ClusterChain *chain, void *destination)
//   load_dir(Fat16 *fat, DirEntry *directory)
//   find_file_in_directory(Fat16 *fat, DirEntry *directory, const char* name)
//
// Dependencies: fat16_helpers.h
// Notes: Acts as the main API for FAT16 access. Can be refactored later into a separate FAT16_API header.
// TODOs: Split public API vs helpers, implement LFN support, signature validation
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



uint32_t cluster_to_lba(Fat16 *fat, uint16_t cluster);
uint32_t bytes_to_sectors(Fat16 *fat, uint32_t bytes);
uint16_t sectors_to_clusters(Fat16 *fat, uint32_t sectors);
bool read_cluster(Fat16 *fat, uint16_t cluster, uint8_t *dest);

/**
 * @brief Initialize a FAT16 filesystem instance.
 *
 * Reads the boot sector, extracts BPB fields, computes important on-disk
 * locations (FAT, root directory, data region), and loads the FAT table
 * and root directory into memory.
 *
 * @param fat Pointer to an allocated Fat16 structure.
 *
 * @return 0 on success.
 * @return 1 if boot sector read failed.
 * @return 2 if FAT loading failed.
 * @return 3 if root directory loading failed.
 *
 * @note This function assumes a valid FAT16 volume.
 * @todo Validate boot sector and FAT signatures.
 */
uint8_t fat_init(Fat16 *fat);
/**
 * @brief Read the FAT16 boot sector from disk.
 *
 * Reads sector 0 into the Fat16 boot sector structure.
 *
 * @param fat Pointer to Fat16 structure to populate.
 *
 * @return false on success, true on failure.
 *
 * @note Uses PIO28 ATA reads.
 */
bool read_boot_sector(Fat16 *fat);
/**
 * @brief Load the FAT table into memory.
 *
 * Allocates memory for the FAT and reads all FAT sectors from disk.
 *
 * @param fat Pointer to initialized Fat16 structure.
 *
 * @return false on success, true on failure.
 *
 * @warning Caller is responsible for freeing fat->fat.
 */
bool load_fat(Fat16 *fat);
/**
 * @brief Load the root directory into memory.
 *
 * Allocates memory and reads the entire FAT16 root directory region.
 *
 * @param fat Pointer to initialized Fat16 structure.
 *
 * @return false on success, true on failure.
 *
 * @warning Caller is responsible for freeing fat->root_dir.
 */
bool load_root_directory(Fat16 *fat);
/**
 * @brief Find a file in the root directory by 8.3 name.
 *
 * Searches the in-memory root directory for a matching 11-byte
 * FAT 8.3 filename.
 *
 * @param fat  Pointer to initialized Fat16 structure.
 * @param name 11-byte FAT filename (space-padded).
 *
 * @return Pointer to DirEntry if found.
 * @return NULL if not found or end-of-directory reached.
 */
DirEntry *find_file(Fat16 *fat, const char* name);
/**
 * @brief Traverse a FAT cluster chain.
 *
 * Follows FAT16 entries starting from a given cluster until
 * an end-of-chain marker is reached.
 *
 * @param fat Pointer to initialized Fat16 structure.
 * @param chain_start First cluster of the chain.
 *
 * @return Pointer to allocated ClusterChain structure.
 * @return NULL on error or corruption.
 *
 * @note Performs basic sanity checks but does not fully detect loops.
 * @warning Caller must free both chain->chain and the ClusterChain itself.
 */
ClusterChain *traverse_chain(Fat16 *fat, uint16_t chain_start);
/**
 * @brief Load an entire cluster chain into memory.
 *
 * Reads all clusters in a chain sequentially into the provided buffer.
 *
 * @param fat Pointer to initialized Fat16 structure.
 * @param chain Cluster chain description.
 * @param destination Destination buffer (must be large enough).
 *
 * @return Destination pointer.
 *
 * @todo Return number of bytes or sectors read.
 */
void *load_cluster_chain(Fat16 *fat, ClusterChain *chain, void *destination);
/**
 * @brief Load a directory from disk into memory.
 *
 * Reads all clusters belonging to a directory entry and returns
 * a pointer to a newly allocated directory table.
 *
 * @param fat Pointer to initialized Fat16 structure.
 * @param directory Directory entry describing the directory.
 *
 * @return Pointer to allocated DirEntry array.
 * @return NULL on error.
 *
 * @warning Caller must free the returned buffer.
 */
DirEntry *load_dir(Fat16 *fat, DirEntry *directory);
/**
 * @brief Find a file inside a directory.
 *
 * Searches either the root directory or a subdirectory for a file
 * matching a given 8.3 filename.
 *
 * @param fat Pointer to initialized Fat16 structure.
 * @param directory Directory entry to search (root if cluster = 0).
 * @param name 11-byte FAT filename.
 *
 * @return Newly allocated DirEntry on success.
 * @return NULL if not found.
 *
 * @warning Caller must free the returned DirEntry.
 */
DirEntry *find_file_in_directory(Fat16 *fat, DirEntry *directory, const char* name);
/**
 * @brief Parse a directory into a high-level structure.
 *
 * Converts raw FAT directory entries (SFN and LFN) into a list
 * of parsed directory entries with resolved filenames.
 *
 * @param dir Pointer to directory entries.
 * @param max_entries Maximum number of entries to parse.
 *
 * @return Pointer to allocated parsed_dir structure.
 * @return NULL on allocation failure.
 *
 * @warning Caller must free the returned structure using free_parsed_dir().
 */
parsed_dir *parse_dir(Fat16 *fat, DirEntry *dir, uint32_t max_entries);
/**
 * @brief Assemble a full filename from an LFN buffer.
 *
 * Combines parsed LFN entries (stored in reverse order) into
 * a single null-terminated ASCII string.
 *
 * @param buffer Pointer to populated LFN_buffer.
 *
 * @return Allocated filename string.
 * @return NULL if buffer is empty or allocation fails.
 *
 * @warning Caller must free the returned string.
 */
char *parse_LFN_buffer(LFN_buffer *buffer);
/**
 * @brief Parse a single Long File Name (LFN) directory entry.
 *
 * Extracts UTF-16 name fragments and stores them in an LFN buffer.
 *
 * @param lfn Pointer to on-disk LFN entry.
 * @param buf Pointer to LFN_buffer accumulator.
 *
 * @return true on success.
 * @return false if buffer is full.
 */
bool parse_LFN_entry(const LFN_entry *lfn, LFN_buffer *buf);
/**
 * @brief Free a parsed directory structure.
 *
 * Frees all allocated filenames, entry arrays, and the directory itself.
 *
 * @param pd Pointer to parsed_dir structure.
 */
void free_parsed_dir(parsed_dir *pd);
/**
 * @brief Find an entry in a parsed directory by name.
 *
 * Searches through a parsed_dir structure for an entry whose
 * resolved filename matches the given name.
 *
 * @param dir  Pointer to a parsed_dir structure.
 * @param name Null-terminated filename to search for.
 *
 * @return Pointer to the matching parsed_dir_entry if found.
 * @return NULL if no matching entry exists.
 *
 * @note Comparison is case-sensitive and assumes valid null-terminated names.
 */
parsed_dir_entry *find_parsed_dir_entry(parsed_dir *dir, const char *name);

void *load_file(Fat16 *fat, DirEntry *file);

void *open(Fat16 *fat, const char *name);
bool __reload_open(Fat16 *fat, bool *dir_owned, DirEntry **current_dir, parsed_dir **current_parsed_dir, parsed_dir_entry *e);
uint32_t round_bytes_to_clusters(Fat16 *fat, uint32_t bytes);
#endif
