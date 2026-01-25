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
#include "fat16_helpers.h"

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
parsed_dir *parse_dir(DirEntry *dir, uint32_t max_entries);
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

bool __reload_open(Fat16 *fat, bool *dir_owned, DirEntry **current_dir, parsed_dir **current_parsed_dir, parsed_dir_entry *e);
#endif
