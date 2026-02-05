#ifndef FAT16_API_H
#define FAT16_API_H

#include "fat16_defs.h"



fat16_error_type get_fat16_last_error();
uint8_t fat_init(Fat16 *fat);
DirEntry *find_file(Fat16 *fat, const char* name);



void *open(Fat16 *fat, const char *name);
#endif
