// ===== FILE: fat16_defs.h =====
// Purpose: Defines FAT16 constants, macros, and maximum limits used throughout the FAT16 implementation.
//
// #Defines / Constants:
//   INVALID_LBA       - 0xFFFFFFFF, returned for invalid cluster-to-LBA conversions
//   FAT_EOC_START     - 0xFFF8, first value marking end-of-chain in FAT
//   FAT_BAD_SECTOR    - 0xFFF7, marks a bad cluster in FAT
//   FAT_FREE_SECTOR   - 0x0000, marks a free cluster in FAT
//   MAX_DIR_ENTRIES   - 256, max entries in a directory
//   MAX_NAME_LEN      - 256, max name length for directories or LFN
//   MAX_LFN           - 256, max long filename characters (including null terminator)
//
// Dependencies: bool.h, int.h (from STD)
// Notes: Used globally for both in-memory and on-disk FAT16 structures and helpers
// TODO: None in this file

#ifndef FAT16_DEFS_H
#define FAT16_DEFS_H

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
#endif