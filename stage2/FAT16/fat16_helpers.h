// ===== FILE: fat16_helpers.h =====
// Purpose: Declares helper functions for low-level FAT16 operations.
//
// Functions (Declared):
//   uint32_t cluster_to_lba(Fat16 *fat, uint16_t cluster);
//   uint32_t bytes_to_sectors(Fat16 *fat, uint32_t bytes);
//   uint16_t sectors_to_clusters(Fat16 *fat, uint32_t sectors);
//   bool read_cluster(Fat16 *fat, uint16_t cluster, uint8_t *dest);
//
// Dependencies: fat16_structures.h
// Notes: Provides utility functions used by higher-level FAT16 operations. Does not handle directories or files.
// TODO: Implement error checking in read_cluster
#ifndef FAT16_HELPERS_H
#define FAT16_HELPERS_H

#include "fat16_structures.h"


uint32_t cluster_to_lba(Fat16 *fat, uint16_t cluster);
uint32_t bytes_to_sectors(Fat16 *fat, uint32_t bytes);
uint16_t sectors_to_clusters(Fat16 *fat, uint32_t sectors);
bool read_cluster(Fat16 *fat, uint16_t cluster, uint8_t *dest);

#endif