#include "ATA.h"
#include "../UTILS/asm_utils.h"
typedef enum ata_mode
{
    chs = 0,
    lba
} ata_mode;
typedef enum ata_drive_type
{
    master = 0,
    slave
} ata_drive_type;


typedef struct DEVICE_EXTENDED_28
{
    uint8_t sector_count;
    uint8_t lba_low;
    uint8_t lba_mid;
    uint8_t lba_high;
    uint8_t dev_sel;
    uint8_t command;
} DEVICE_EXTENDED_28;

void flush_command(DEVICE* drv, DEVICE_EXTENDED_28* d)
{
    outb(drv->base + REG_DEVSEL, d->dev_sel);
    ata_wait_400ns(drv->base);
    outb(drv->base + REG_SECCOUNT, d->sector_count);
    outb(drv->base + REG_LBA_LO, d->lba_low);
    outb(drv->base + REG_LBA_MID, d->lba_mid);
    outb(drv->base + REG_LBA_HI, d->lba_high);
    outb(drv->base + REG_COMMAND, d->command);
}
typedef enum ata_command
{
    NOP = 0x00,
    CFA_REQUEST_EXTENDED_ERROR_CODE = 0x03,
    DATA_SET_MANAGEMENT = 0x06,
    DATA_SET_MANAGEMENT_XL = 0x07,
    DEVICE_RESET = 0x08,
    REQUEST_SENSE_DATA_EXT = 0x0B,
    RECALIBRATE = 0x10,
    RECALIBRATE1 = 0x11,
    GET_PHYSICAL_ELEMENT_STATUS = 0x12,
    RECALIBRATE2 = 0x12,
    RECALIBRATE3 = 0x13,
    RECALIBRATE4 = 0x14,
    RECALIBRATE5 = 0x15,
    RECALIBRATE6 = 0x16,
    RECALIBRATE7 = 0x17,
    RECALIBRATE8 = 0x18,
    RECALIBRATE9 = 0x19,
    RECALIBRATE10 = 0x1A,
    RECALIBRATE11 = 0x1B,
    RECALIBRATE12 = 0x1C,
    RECALIBRATE13 = 0x1D,
    RECALIBRATE14 = 0x1E,
    RECALIBRATE15 = 0x1F,
    READ_SECTOR_S = 0x20,
    READ_SECTOR_S_NO_RETRY = 0x21,
    READ_LONG = 0x22,
    READ_LONG_NO_RETRY = 0x23,
    READ_SECTOR_S_EXT = 0x24,


    

} ata_command;


int __pio28_read__set_LBA(DEVICE_EXTENDED_28* drv, uint32_t lba)
{
    drv->lba_low =  (lba & 0xFF);
    drv->lba_mid =  ((lba >> 8) & 0xFF);
    drv->lba_high = ((lba >> 16) & 0xFF);
    drv->dev_sel = drv->dev_sel | ((lba >> 24) & 0x0F);
}

int __pio28_read__set_Sec_count(DEVICE_EXTENDED_28* drv, uint32_t sec_count)
{
    drv->sector_count = (sec_count == 256 ? 0 : (uint8_t)sec_count);
}

int __set_ata_mode(DEVICE_EXTENDED_28* drv, ata_mode m)
{
    switch (m)
    {
    case lba:
        drv->dev_sel = (drv->dev_sel | 64);
        break;
    case chs:
        // drv->dev_sel = (drv->dev_sel  ^ 64);
        drv->dev_sel &= ~0x40; // CHS

        break;
    
    default:
        // TODO: raise appropriate error.
        break;
    }

}

int __ata_pio28_set_dev_sel(DEVICE_EXTENDED_28* drv, ata_drive_type d)
{
    switch (d)
    {
    case master:
        drv->dev_sel = (0xE0 | drv->dev_sel); // master
        break;
    case slave:
        drv->dev_sel = (0xF0 | drv->dev_sel); // slave
        break;
    
    default:
        break;
    }
}

int __ata_set_command(DEVICE_EXTENDED_28* drv, ata_command c)
{
    drv->command = c;
}

void store_incoming_bytes(DEVICE *drv, uint16_t count, void *buf)
{
        for (int i = 0; i < count; i++) {  // 256 words = 512 bytes
            uint16_t data;
            __asm__ volatile ("inw %1, %0"
                            : "=a"(data)
                            : "Nd"(((DEVICE*)drv)->base + REG_DATA));

            *(uint8_t*)buf++ = (uint8_t)(data & 0xFF);       // low byte
            *(uint8_t*)buf++ = (uint8_t)(data >> 8);         // high byte
        }
}

int pio28_read(void *dest, DEVICE *drv, uint32_t sectors, uint16_t io_base, uint32_t abs_lba) {
    ///* *((uint32_t*)drv + /* offset dd_stLBA */ 0);*/
    if (sectors <= 0)
    {
        set_ata_last_error(INVALID_ARGUMENT);
        return -1;
    }
    uint8_t al;

    DEVICE_EXTENDED_28 d = {0};

    //set mode to lba.
    __set_ata_mode(&d, lba);
    // send sector count
    __pio28_read__set_Sec_count(&d, sectors);

    // send LBA low/mid/high bytes
    __pio28_read__set_LBA(&d, abs_lba);

    // drive/head select
    __ata_pio28_set_dev_sel(&d, master);

    // command
    __ata_set_command(&d, READ_SECTOR_S);

    // flush it all.
    flush_command(drv, &d);
    
    // wait for BSY clear and DRQ set
    ata_wait_not_bsy(drv->base);


    uint8_t *buf = (uint8_t *)dest;

    for (uint32_t s = 0; s < sectors; s++) {
        ata_wait_400ns(drv->base);
        // wait for DRQ for THIS sector
        switch (ata_wait_drq(drv->base))
        {
        case -1:
            set_ata_last_error(ERROR_WHILE_DRQ_POLL);
            break;
        case -2:
            set_ata_last_error(DRQ_TIMEOUT);
            break;
        
        default:
            break;
        }

        store_incoming_bytes(drv, 256, buf);
        buf += 512;
    }
}
