#ifndef ATA_DRIVER
#define ATA_DRIVER
#include "../STD/int.h"



#include "ata_defs.h"




// extern void ata_soft_reset(uint16_t dev_ctl);
void ata_soft_reset(uint16_t dcr_port); 

const char *devtype_to_string(int type);
typedef struct DEVICE {
    uint16_t base;     /* Command block base (e.g. 0x1F0 or 0x170) */
    uint16_t dev_ctl;  /* Control block base (e.g. 0x3F6 or 0x376) */
} DEVICE;

extern DEVICE ata_primary;

int detect_devtype (int slavebit, DEVICE *ctrl);
void ata_soft_reset(uint16_t dcr_port);
void ata_init();
int ata_identify(uint16_t *identify_result);
int test_floating();
void clear_HOB_bits();
int test_if_drive_exists();
void read_sector(uint32_t lba, void *dest);
void read_bytes(void *dest, uint32_t bytes, uint32_t lba_start);
void pio28_read(void *dest, void *drv, uint32_t sectors, uint16_t io_base, uint32_t abs_lba);
void ata_wait_400ns(uint16_t io_base);


void read_bytes(void *dest, uint32_t sectors, uint32_t lba);
#endif