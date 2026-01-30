#ifndef ATA_DRIVER_H
#define ATA_DRIVER_H
#include "ata_helpers.h"
#include "ata_error.h"
#include "ata_identify_result.h"




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
int ata_init();
int ata_identify(DEVICE *drv, ata_identify_u *identify_result);
int test_if_drive_exists();
int pio28_read(void *dest, DEVICE *drv, uint32_t sectors, uint16_t io_base, uint32_t abs_lba);
void ata_wait_400ns(uint16_t io_base);
int ata_get_last_error(void);
void ata_soft_reset(uint16_t dcr_port);
#endif