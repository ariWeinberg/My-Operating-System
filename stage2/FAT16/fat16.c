#include "fat16.h"
#include "../ATA_DRIVER/ATA.h"
#include "../MEMORY_MANAGMENT/memory_managment.h"
#include "../UTILS/utils.h"
#include "../STRING/string.h"
uint8_t fat_init(Fat16 *fat)
{
    if (read_boot_sector(fat))
    {
        return fat16_fail(FAT16_BOOT_SECTOR_READ_FAIL);
    }
    if (fat->bs.boot_signature != 0xAA55)
    {
        return fat16_fail(FAT16_INVALID_BOOT_SIGNATURE);
    }
    if ((fat->bs.ebr.signature != 0x28) && (fat->bs.ebr.signature != 0x29))
    {
        // NOTE: Some FAT variants may not set EBR signature (relaxed later)
        return fat16_fail(FAT16_INVALID_EBR_SIGNATURE);
    }
    if (fat->bs.bpb.Bytes_per_sector == 0 || fat->bs.bpb.sectors_per_cluster == 0 || fat->bs.bpb.sectors_per_FAT == 0)
    {
        return fat16_fail(FAT16_CRITICAL_BPB_VALUE_IS_ZERO);
    }
    
    fat->bytes_per_sector = fat->bs.bpb.Bytes_per_sector;
    fat->sectors_per_cluster = fat->bs.bpb.sectors_per_cluster;

    fat->fat_sectors = fat->bs.bpb.sectors_per_FAT;
    fat->fat_bytes = fat->fat_sectors * fat->bytes_per_sector;
    fat->fat_start_lba = fat->bs.bpb.reserved_sectors;

    fat->root_dir_start_lba = fat->bs.bpb.reserved_sectors + (fat->bs.bpb.FAT_count * fat->bs.bpb.sectors_per_FAT);
    fat->root_dir_bytes = fat->bs.bpb.root_dir_entries * sizeof(DirEntry);
    fat->root_dir_sectors = fat->root_dir_bytes / fat->bytes_per_sector;
    if(fat->root_dir_bytes % fat->bytes_per_sector)
    {fat->root_dir_sectors ++;}
    
    fat->data_start_lba =
    fat->root_dir_start_lba + fat->root_dir_sectors;

    
    if(load_fat(fat))
    {
        free(fat->fat);
        return fat16_fail(FAT16_FAT_LOAD_FAIL);
    }
    if(load_root_directory(fat))
    {
        free(fat->fat);
        free(fat->root_dir);
        return fat16_fail(FAT16_ROOT_DIRECTORY_LOAD_FAIL);
    }

    return fat16_ok();
}

bool read_boot_sector(Fat16 *fat)
{
    pio28_read(&(fat->bs), &ata_primary, 1, 0);
    return false;
}

bool load_fat(Fat16 *fat)
{
    
    fat->fat = zalloc(fat->fat_sectors * fat->bytes_per_sector);
    
    // Fat16 entries are little-endian; code assumes LE CPU
    pio28_read(fat->fat, &ata_primary, fat->fat_sectors, fat->fat_start_lba);
    return false;
}

bool load_root_directory(Fat16 *fat)
{
    fat->root_dir = zalloc(fat->root_dir_sectors * fat->bytes_per_sector);
    pio28_read(fat->root_dir, &ata_primary, fat->root_dir_sectors, fat->root_dir_start_lba);
    return false;
}
