// ===== FILE: fat16_helpers.c =====
// Purpose: Implements the FAT16 helper functions declared in fat16_helpers.h.
//
// Functions (Defined):
//   cluster_to_lba      - convert cluster number to LBA
//   bytes_to_sectors    - convert bytes to number of sectors
//   sectors_to_clusters - convert sectors to cluster count
//   read_cluster        - read a single cluster into memory
//
// Dependencies: fat16_structures.h, ata_driver.h, memory_managment.h
// Notes: Core utility layer, performs low-level reads and conversions
// TODOs:
//   - Implement error handling in read_cluster
//   - Implement LFN support
//   - Implement FAT mirroring, validation, loop detection
#include "fat16_structures.h"

#include "fat16_helpers.h"

#include "../ATA_DRIVER/ATA.h"
#include "../MEMORY_MANAGMENT/memory_managment.h"

// 
// TODO:
// implement LFN.
// implement volume label handeling.
// implement . and .. handeling.
// implement FAT miroring.
// implement loop detection.
// implement validation checks: boot signature, FAT signature (EBR), FAT type.

uint32_t cluster_to_lba(Fat16 *fat, uint16_t cluster)
{
    if(cluster < 2)
    {
        return INVALID_LBA;
    }

    return fat->data_start_lba + ((cluster - 2) * fat->sectors_per_cluster);
}

uint32_t bytes_to_sectors(Fat16 *fat, uint32_t bytes)
{
    
    uint32_t sectors = (bytes / fat->bytes_per_sector);
    if(bytes % fat->bytes_per_sector)
    {sectors++;}
    return sectors;
}

uint16_t sectors_to_clusters(Fat16 *fat, uint32_t sectors)
{
    uint16_t cluster_count = sectors / fat->sectors_per_cluster;
    if(sectors % fat->sectors_per_cluster)
    {
        cluster_count++;
    }
    return cluster_count;
}

bool read_cluster(Fat16 *fat, uint16_t cluster, uint8_t *dest)
{
    uint32_t lba = cluster_to_lba(fat, cluster);
    if(lba == INVALID_LBA)
    {
        return true;
    }
    pio28_read(dest, &ata_primary, fat->sectors_per_cluster, ATA_PRIMARY_BASE, lba);
    // 
    // TODO: implement error checking and handleing...
    // 
    return false;
}