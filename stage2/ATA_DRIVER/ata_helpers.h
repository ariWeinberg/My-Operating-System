#ifndef ATA_HELPERS_H
#define ATA_HELPERS_H
#include "../STD/int.h"
#include "ata_defs.h"

uint32_t ata_u32(uint16_t lo, uint16_t hi);
int ata_wait_not_bsy(uint16_t io_base);
int ata_wait_drq(uint16_t io_base);
const char *devtype_to_string(int type);
#endif