#include "ata_driver.h"
#include "../UTILS/asm_utils.h"


int ata_identify(DEVICE *drv, ata_identify_u *identify_result)
{
    set_ata_last_error(ATA_ERR_NONE);

    if (!identify_result) {
        set_ata_last_error(ATA_ERR_INVALID_ARG);
        return -1;
    }

    outb(drv->base + REG_DEVSEL, 0xA0);
    ata_wait_400ns(drv->base);

    outb(drv->base + REG_SECCOUNT, 0);
    outb(drv->base + REG_LBA_LO,  0);
    outb(drv->base + REG_LBA_MID,  0);
    outb(drv->base + REG_LBA_HI,  0);

    outb(drv->base + REG_COMMAND, 0xEC);

    uint8_t status = inb(drv->base + REG_STATUS);
    if (status == 0) {
        set_ata_last_error(ATA_ERR_NO_DEVICE);
        return -1;
    }

    int rc = ata_wait_not_bsy(drv->base);
    if (rc == -1) {
        set_ata_last_error(ATA_ERR_DEVICE_FAULT);
        return -1;
    }
    if (rc == -2) {
        set_ata_last_error(ATA_ERR_TIMEOUT);
        return -1;
    }

    uint8_t cl = inb(drv->base + REG_LBA_MID);
    uint8_t ch = inb(drv->base + REG_LBA_HI);
    if (cl != 0 || ch != 0) {
        set_ata_last_error(ATA_ERR_NOT_ATA);
        return -1;
    }

    rc = ata_wait_drq(drv->base);
    if (rc == -1) {
        set_ata_last_error(ATA_ERR_DEVICE_FAULT);
        return -1;
    }
    if (rc == -2) {
        set_ata_last_error(ATA_ERR_TIMEOUT);
        return -1;
    }

    /* Read IDENTIFY data */
    for (int i = 0; i < 256; i++) {
        identify_result->raw[i] =
            inw(drv->base + REG_DATA);
    }

    return 0;
}

