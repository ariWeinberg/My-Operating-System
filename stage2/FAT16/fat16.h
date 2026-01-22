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


uint8_t fat_init(Fat16 *fat);
bool read_boot_sector(Fat16 *fat);
bool load_fat(Fat16 *fat);
bool load_root_directory(Fat16 *fat);
DirEntry *find_file(Fat16 *fat, const char* name);
ClusterChain *traverse_chain(Fat16 *fat, uint16_t chain_start);
void *load_cluster_chain(Fat16 *fat, ClusterChain *chain, void *destination);
DirEntry *load_dir(Fat16 *fat, DirEntry *directory);
DirEntry *find_file_in_directory(Fat16 *fat, DirEntry *directory, const char* name);
parsed_dir *parse_dir(DirEntry *dir, uint32_t max_entries);
char *parse_LFN_buffer(LFN_buffer *buffer);
bool parse_LFN_entry(const LFN_entry *lfn, LFN_buffer *buf);
void free_parsed_dir(parsed_dir *pd);

#endif