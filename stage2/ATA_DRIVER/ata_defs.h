#ifndef ATA_DEFS_H
#define ATA_DEFS_H

#define ATA_PRIMARY_BASE     0x1F0
#define ATA_PRIMARY_DEVCTL   0x3F6

/* differentiate ATA, ATAPI, SATA and SATAPI */
#define ATADEV_PATAPI 5
#define ATADEV_SATAPI 4
#define ATADEV_PATA 3
#define ATADEV_SATA 2
#define ATADEV_UNKNOWN 1


/* ATA command block register offsets */
#define REG_DATA        0   /* 0x1F0 */
#define REG_ERROR       1   /* 0x1F1 (read) */
#define REG_FEATURES    1   /* 0x1F1 (write) */
#define REG_SECCOUNT    2   /* 0x1F2 */
#define REG_LBA_LO      3   /* 0x1F3 */
#define REG_CYL_LO      4   /* 0x1F4 */
#define REG_CYL_HI      5   /* 0x1F5 */
#define REG_DEVSEL      6   /* 0x1F6 */
#define REG_STATUS      7   /* 0x1F7 (read) */
#define REG_COMMAND     7   /* 0x1F7 (write) */

#define NO_ATA_IDENTIFY_DEBUG_PRINT_INFO
#define NO_ATA_IDENTIFY_DEBUG_PRINT_ERR

#endif