#include "ATA.h"
#include "../UTILS/asm_utils.h"



int flush_command(DEVICE* drv, DEVICE_EXTENDED_28* d)
{
    if (ata_wait_not_bsy(drv->base) < 0)
        return -1;
    uint8_t status = inb(drv->base + REG_STATUS);
    if (status & 0x01) return ata_fail(ERROR);
    if (status & 0x20) return ata_fail(DEVICE_FAULT);

    outb(drv->base + REG_DEVSEL, d->dev_sel);
    ata_wait_400ns(drv->base);
    outb(drv->base + REG_SECCOUNT, d->sector_count);
    outb(drv->base + REG_LBA_LO, d->lba_low);
    outb(drv->base + REG_LBA_MID, d->lba_mid);
    outb(drv->base + REG_LBA_HI, d->lba_high);
    outb(drv->base + REG_COMMAND, d->command);
    return ata_ok();
}


int __pio28_read__set_LBA(DEVICE_EXTENDED_28* drv, uint32_t lba)
{
    if (lba & 0xF0000000)
        return ata_fail(INVALID_ARGUMENT); // beyond 28-bit

    drv->lba_low =  (lba & 0xFF);
    drv->lba_mid =  ((lba >> 8) & 0xFF);
    drv->lba_high = ((lba >> 16) & 0xFF);
    
    drv->dev_sel &= 0xF0;                 // clear low nibble
    drv->dev_sel |= (lba >> 24) & 0x0F;   // set LBA[27:24]

    return ata_ok();
}

int __pio28_read__set_Sec_count(DEVICE_EXTENDED_28 *d, uint32_t sec_count)
{
    if (sec_count == 0 || sec_count > 256)
        return ata_fail(INVALID_ARGUMENT);

    d->sector_count = (sec_count == 256) ? 0 : (uint8_t)sec_count;
    return ata_ok();
}

int __set_ata_mode(DEVICE_EXTENDED_28* drv, ata_mode m)
{
    switch (m)
    {
    case lba:
        drv->dev_sel |= 0x40;
        return ata_ok();
    case chs:
        // drv->dev_sel = (drv->dev_sel  ^ 64);
        drv->dev_sel &= ~0x40; // CHS
        return ata_ok();
    
    default:
        return ata_fail(INVALID_ARGUMENT);
    }

}

int __ata_pio28_set_dev_sel(DEVICE_EXTENDED_28 *d, ata_drive_type drive)
{
    d->dev_sel &= 0x4F;   // clear drive + high LBA bits
    d->dev_sel |= 0xE0;   // mandatory 111xxxxx

    if (drive == slave)
        d->dev_sel |= 0x10;

    return ata_ok();
}


int __ata_set_command(DEVICE_EXTENDED_28* drv, ata_command c)
{
    drv->command = c;
    return ata_ok();
}

int store_incoming_bytes(DEVICE *drv, uint16_t words, void *buf)
{
    if (!drv || !buf)
        return ata_fail(INVALID_ARGUMENT);

    uint8_t status;

    /* Final sanity check before data phase */
    status = inb(drv->base + REG_STATUS);

    if (status & 0x01)   /* ERR */
        return ata_fail(ERROR);

    if (status & 0x20)   /* DF */
        return ata_fail(DEVICE_FAULT);

    if (!(status & 0x08)) /* DRQ */
        return ata_fail(ERROR_WHILE_DRQ_POLL);
    if (status == 0xFF)
        return ata_fail(BUS_FLOAT);


    for (uint16_t i = 0; i < words; i++) {
        uint16_t data;

        __asm__ volatile ("inw %1, %0"
                          : "=a"(data)
                          : "Nd"(drv->base + REG_DATA));

        *(uint8_t *)buf++ = data & 0xFF;
        *(uint8_t *)buf++ = data >> 8;
    }

    return ata_ok();
}


int pio28_read(void *dest, DEVICE *drv, uint32_t sectors, uint32_t abs_lba) {
    ///* *((uint32_t*)drv + /* offset dd_stLBA */ 0);*/
    if (sectors == 0)
    {
        return ata_fail(INVALID_ARGUMENT);
    }
    if (!dest || !drv)
        return ata_fail(INVALID_ARGUMENT);

    DEVICE_EXTENDED_28 d = {0};

    if (__set_ata_mode(&d, lba) < 0) return -1;
    if (__pio28_read__set_Sec_count(&d, sectors) < 0) return -1;
    if (__pio28_read__set_LBA(&d, abs_lba) < 0) return -1;
    if (__ata_pio28_set_dev_sel(&d, master) < 0) return -1;
    if (__ata_set_command(&d, READ_SECTOR_S) < 0) return -1;

    if (flush_command(drv, &d) < 0)
        return -1;


    uint8_t *buf = (uint8_t *)dest;

    for (uint32_t s = 0; s < sectors; s++) {
        ata_wait_400ns(drv->base);
        // wait for DRQ for THIS sector
        if (ata_wait_drq(drv->base) < 0)
            {
                return -1;
            }

        if (store_incoming_bytes(drv, 256, buf) < 0)
            return -1;

        buf += 512;
    }
    return ata_ok();
}
