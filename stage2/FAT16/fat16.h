#ifndef FAT16_H
#define FAT16_H
#include "fat16_defs.h"

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
DirEntry *find_file_in_directory(Fat16 *fat, DirEntry *directory, const char* name);
parsed_dir *parse_dir(Fat16 *fat, DirEntry *dir, uint32_t max_entries);
char *parse_LFN_buffer(LFN_buffer *buffer);
bool parse_LFN_entry(const LFN_entry *lfn, LFN_buffer *buf);
void free_parsed_dir(parsed_dir *pd);
parsed_dir_entry *find_parsed_dir_entry(parsed_dir *dir, const char *name);

void lfn_buffer_clear(LFN_buffer *buf);

void *load_clustered_entry(Fat16 *fat, DirEntry *entry);

char *normalize_SFN(const char sfn[11]);

void *open(Fat16 *fat, const char *name);
bool __reload_open(Fat16 *fat, bool *dir_owned, DirEntry **current_dir, parsed_dir **current_parsed_dir, parsed_dir_entry *e);
uint32_t round_bytes_to_clusters(Fat16 *fat, uint32_t bytes);
#endif
