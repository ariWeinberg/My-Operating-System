#include "ATA.h"
#include "../UTILS/asm_utils.h"

#define ATA_RESET_TIMEOUT 100000

int ata_soft_reset(uint16_t dcr_port)
{
    uint8_t status;
    uint32_t timeout;

    if (dcr_port == 0)
        return ata_fail(INVALID_ARGUMENT);

    /* trigger software reset */
    outb(dcr_port, 4);
    outb(dcr_port, 0);

    /* 400ns delay */
    inb(dcr_port);
    inb(dcr_port);
    inb(dcr_port);
    inb(dcr_port);

    /* wait for BSY=0 and DRDY=1 */
    for (timeout = ATA_RESET_TIMEOUT; timeout > 0; timeout--) {
        status = inb(dcr_port);

        if (status == 0xFF)
            return ata_fail(BUS_FLOAT);

        /* BSY clear, DRDY set */
        if ((status & 0xC0) == 0x40)
            return ata_ok();
    }

    return ata_fail(BSY_TIMEOUT);
}
