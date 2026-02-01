#include "ATA.h"
#include "../UTILS/asm_utils.h"


uint32_t ata_u32(uint16_t lo, uint16_t hi)
{
    return ((uint32_t)hi << 16) | lo;
}

uint64_t ata_u64(uint16_t w0, uint16_t w1,
                               uint16_t w2, uint16_t w3)
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

        if (status & 0x01)   /* ERR */
            return -1;

        if (!(status & 0x80)) /* BSY cleared */
            return 0;
    }
    return -2; /* timeout */
}

int ata_wait_drq(uint16_t io_base)
{
    uint32_t timeout = ATA_TIMEOUT_DRQ;
    uint8_t status;

    while (timeout--)
    {
        status = inb(io_base + REG_STATUS);

        if (status & 0x01)   /* ERR */
            return -1;

        if (status & 0x08)  /* DRQ */
            return 0;
    }
    return -2; /* timeout */
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

void ata_wait_400ns(uint16_t io_base)
{
    inb(io_base + REG_STATUS); inb(io_base + REG_STATUS); inb(io_base + REG_STATUS); inb(io_base + REG_STATUS);
}