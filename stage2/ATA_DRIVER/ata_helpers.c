#include "ATA.h"
#include "../UTILS/asm_utils.h"


uint32_t ata_u32(uint16_t lo, uint16_t hi)
{
    return ((uint32_t)hi << 16) | lo;
}

uint64_t ata_u64(uint16_t w0, uint16_t w1, uint16_t w2, uint16_t w3)
{
    return ((uint64_t)w3 << 48) |
           ((uint64_t)w2 << 32) |
           ((uint64_t)w1 << 16) |
           (uint64_t)w0;
}

int ata_wait_not_bsy(uint16_t io_base)
{
    uint32_t timeout = ATA_TIMEOUT_BSY;
    uint8_t status;

    while (timeout--)
    {
        status = inb(io_base + REG_STATUS);

        if (status == 0xFF) return ata_fail(BUS_FLOAT);  // no device
        if (status & 0x01)  return ata_fail(ERROR);      // ERR
        if (!(status & 0x80)) return ata_ok();          // BSY cleared
    }

    return ata_fail(BSY_TIMEOUT);
}

int ata_wait_drq(uint16_t io_base)
{
    uint32_t timeout = ATA_TIMEOUT_DRQ;
    uint8_t status;

    while (timeout--)
    {
        status = inb(io_base + REG_STATUS);

        if (status == 0xFF) return ata_fail(BUS_FLOAT);  // no device
        if (status & 0x01)  return ata_fail(ERROR);      // ERR
        if (status & 0x08)  return ata_ok();             // DRQ set
    }

    return ata_fail(DRQ_TIMEOUT);
}


const char *devtype_to_string(int type) {
    switch (type) {
        case ATADEV_PATA:   return "PATA";
        case ATADEV_SATA:   return "SATA";
        case ATADEV_PATAPI: return "PATAPI";
        case ATADEV_SATAPI: return "SATAPI";
        default:            return "UNKNOWN";
    }
}

inline void ata_wait_400ns(uint16_t io_base)
{
    inb(io_base + REG_STATUS); inb(io_base + REG_STATUS); inb(io_base + REG_STATUS); inb(io_base + REG_STATUS);
}