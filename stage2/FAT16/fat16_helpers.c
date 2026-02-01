#include "fat16.h"

#include "../ATA_DRIVER/ATA.h"
#include "../MEMORY_MANAGMENT/memory_managment.h"

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
    pio28_read(dest, &ata_primary, fat->sectors_per_cluster, lba);
    // 
    // TODO: implement error checking and handleing...
    // 
    return false;
}

uint32_t round_bytes_to_clusters(Fat16 *fat, uint32_t bytes)
{
    return (sectors_to_clusters(fat, bytes_to_sectors(fat, bytes)) * fat->bytes_per_sector * fat->sectors_per_cluster);
}