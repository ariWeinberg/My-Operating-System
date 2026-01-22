
#include "ata_driver.h"
#include "../UTILS/asm_utils.h"
#include "../UTILS/utils.h"
#include "../SCREEN_DRIVER/screen_driver.h"


int print_level = 0;

void ata_init();
int ata_identify(uint16_t *identify_result);
int test_floating();
void clear_HOB_bits();
void read_bytes(void *dest, uint32_t sectors, uint32_t lba);
void pio28_read(void *dest, void *drv, uint32_t sectors, uint16_t io_base, uint32_t abs_lba);
int test_if_drive_exists();
void read_sector(uint32_t lba, void *dest);
void ata_wait_400ns(uint16_t io_base);

DEVICE ata_primary = {
    .base    = ATA_PRIMARY_BASE,
    .dev_ctl = ATA_PRIMARY_DEVCTL
};

// extern void ata_soft_reset(uint16_t dev_ctl);
void ata_soft_reset(uint16_t dcr_port); 

const char *devtype_to_string(int type) {
    switch (type) {
        case ATADEV_PATA:   return "PATA";
        case ATADEV_SATA:   return "SATA";
        case ATADEV_PATAPI: return "PATAPI";
        case ATADEV_SATAPI: return "SATAPI";
        default:            return "UNKNOWN";
    }
}

/* on Primary bus: ctrl->base =0x1F0, ctrl->dev_ctl =0x3F6. REG_CYL_LO=4, REG_CYL_HI=5, REG_DEVSEL=6 */
int detect_devtype (int slavebit, struct DEVICE *ctrl)
{
    ata_soft_reset(ctrl->dev_ctl);		/* waits until master drive is ready again */
	// outb(ctrl->base + REG_DEVSEL, 0xA0 | slavebit<<4);
    outb(ctrl->base + REG_DEVSEL, 0xA0 | (slavebit << 4));

	inb(ctrl->dev_ctl);			/* wait 400ns for drive select to work */
	inb(ctrl->dev_ctl);
	inb(ctrl->dev_ctl);
	inb(ctrl->dev_ctl);
	unsigned cl=inb(ctrl->base + REG_CYL_LO);	/* get the "signature bytes" */
	unsigned ch=inb(ctrl->base + REG_CYL_HI);

	/* differentiate ATA, ATAPI, SATA and SATAPI */
	if (cl==0x14 && ch==0xEB) return ATADEV_PATAPI;
	if (cl==0x69 && ch==0x96) return ATADEV_SATAPI;
	if (cl==0 && ch == 0) return ATADEV_PATA;
	if (cl==0x3c && ch==0xc3) return ATADEV_SATA;
	return ATADEV_UNKNOWN;
}
/* Single-tasking PIO ATA software reset */
void ata_soft_reset(uint16_t dcr_port) {
    uint8_t al;

    /* write 4 to DCR to trigger software reset */
    outb(dcr_port, 4);

    /* restore bus to normal operation */
    outb(dcr_port, 0);

    /* 400ns delay (4 I/O reads) */
    inb(dcr_port);
    inb(dcr_port);
    inb(dcr_port);
    inb(dcr_port);

    /* wait for BSY clear and DRDY set */
    do {
        al = inb(dcr_port);
        al &= 0xC0;        /* mask BSY (bit 7) and DRDY (bit 6) */
    } while (al != 0x40);   /* loop until BSY=0 and DRDY=1 */
}
void ata_init()
{
    if(print_level > 1){print_string("\n\n\n\rinitializing disk...\n\n\r");}
    clear_HOB_bits();
    if(print_level > 1){print_string("HOB clear\n\r");}
    if(test_floating())
    {
        if(print_level > 0){print_string("device is floating! no device present on buss... exiting initialization!\n\r");}
        return;
    }
    if(print_level > 1){print_string("buss is not floating!!!\n\r");}
    int res = test_if_drive_exists();
    if(res)
    {        
        #ifndef NO_ATA_IDENTIFY_DEBUG_PRINT_ERR
        print_string("err reding values back. ERR: ");
        print_string((char*)res);
        print_string("\n\r");
        #endif

        return;
    }
    if(print_level > 1){print_string("drive exists and is go for IDENTIFY!\n\r");}

}
int ata_identify(uint16_t *identify_result)
{
    if(print_level > 1){print_string("setting up for IDENTIFY command...\n\r");}

    outb(ATA_PRIMARY_BASE + REG_DEVSEL,0xA0);
    
    outb(ATA_PRIMARY_BASE + REG_SECCOUNT, 0x0);
    outb(ATA_PRIMARY_BASE + REG_LBA_LO, 0x0);
    outb(ATA_PRIMARY_BASE + REG_CYL_LO, 0x0);
    outb(ATA_PRIMARY_BASE + REG_CYL_HI, 0x0);
    if(print_level > 1){print_string("sending IDENTIFY command...\n\r");}
    outb(ATA_PRIMARY_BASE + REG_COMMAND, 0xEC);
    if(print_level > 1){print_string("sent IDENTIFY command...\n\r");}
    
    if(print_level > 1){print_string("polling status...\n\r");}
    uint8_t status = inb(ATA_PRIMARY_BASE + REG_STATUS);
    if (!status)
    {
        #ifndef NO_ATA_IDENTIFY_DEBUG_PRINT_ERR
        print_string("err: device does not exist! (the status was zero...\n\r)");
        #endif
        return 1;
    }
    status = inb(ATA_PRIMARY_BASE + REG_STATUS);
    while (1)
    {
        if(inb(ATA_PRIMARY_BASE + REG_CYL_LO) | inb(ATA_PRIMARY_BASE + REG_CYL_HI))
        {
            if(print_level > 0){print_string("drive is not ATA... exiting now!\n\r");}
            return 2;
        }
        if((status & 0x80))
        {
        if(print_level > 1){print_string("busy\n\r");}
        }
        if ((status & 0x1))
        {
            if(print_level > 0){print_string("ERR: err bit set!\n\r");}
            return 3;
        }
        if ((status & 0x8))
        {
        if(print_level > 1){print_string("DRQ bit is set. ready!!!!\n\r");}
            // return 0;
            break;
        }

        status = inb(ATA_PRIMARY_BASE + REG_STATUS);
        status = inb(ATA_PRIMARY_BASE + REG_STATUS);
        status = inb(ATA_PRIMARY_BASE + REG_STATUS);
        status = inb(ATA_PRIMARY_BASE + REG_STATUS);
    }

    for( int i = 0; i < 256; i++)
    {
        identify_result[i] = inw(ATA_PRIMARY_BASE + REG_DATA);
    }



    char buf[11];
    uint32_to_str(combine_u16_to_u32(identify_result[60],identify_result[61]), buf);
    // buf -> "12345"

    if(print_level > 1){
    print_string("\n\n\rmax LBA 28 accessible sectores: ");
    print_string(buf);
    print_string("\n\r");
    }


    return 0;
    

}
int test_floating()
{
    uint8_t status = inb(ATA_PRIMARY_BASE + REG_STATUS);
    if(status == 0xff)
    {
        return 1;
    }
    return 0;
}
void clear_HOB_bits()
{
    uint8_t val = inb(ATA_PRIMARY_DEVCTL);
    // val = val ^ 0b10000000;
    val &= ~0x80;
    outb(ATA_PRIMARY_DEVCTL, val);
}
int test_if_drive_exists()
{
    outb(0x172, 0x8b);
    outb(0x173, 0x73);
    outb(0x174, 0x74);

    if(inb(0x172) != 0x8b)
        {return 1;}
    if(inb(0x173) != 0x73)
        {return 2;}
    if(inb(0x174) != 0x74)
        {return 3;}
    return 0;
}
// Reads a single 512-byte sector from LBA28 into 'dest'
// dest: must be at least 512 bytes
// lba: sector number
void read_sector(uint32_t lba, void *dest)
{
    pio28_read(dest, &ata_primary, 1, ATA_PRIMARY_BASE, lba);
}

void read_bytes(void *dest, uint32_t bytes, uint32_t lba_start)
{
    uint8_t *out = dest;
    uint32_t sector = lba_start;
    uint32_t remaining = bytes;

    while (remaining > 0) {
        uint8_t sector_buf[512];
        read_sector(sector++, sector_buf);

        uint32_t n = (remaining < 512) ? remaining : 512;
        for (uint32_t i = 0; i < n; i++)
            out[i] = sector_buf[i];

        out       += n;
        remaining -= n;
    }
}

// Inputs:
//   esi: pointer to device struct
//   edi: destination buffer pointer
//   ebx: sectors to read (1-256)
//   dx: base I/O port
//   ebp: relative 28-bit LBA
void pio28_read(void *dest, void *drv, uint32_t sectors, uint16_t io_base, uint32_t abs_lba) {
    ///* *((uint32_t*)drv + /* offset dd_stLBA */ 0);*/
    if (sectors <= 0)
    {return;}
    uint8_t al;


    //set mode to lba.
    uint8_t drive_head_select = inb(ATA_PRIMARY_BASE + REG_DEVSEL);
    outb(ATA_PRIMARY_BASE + REG_DEVSEL, drive_head_select | 64);

    if(print_level > 1){char buffer1[11];
    print_string("abs_lba: ");
    print_string(uint32_to_str(abs_lba, buffer1));
    print_string("\n\r");
    }


    // send sector count
    outb(io_base + REG_SECCOUNT, sectors == 256 ? 0 : (uint8_t)sectors);
    if(print_level > 1){print_string("sec count sent.\n\r");}
    // send LBA low/mid/high bytes
    outb(io_base + REG_LBA_LO, abs_lba & 0xFF);
    outb(io_base + REG_CYL_LO, (abs_lba >> 8) & 0xFF);
    outb(io_base + REG_CYL_HI, (abs_lba >> 16) & 0xFF);
    if(print_level > 1){print_string("lba sent.\n\r");}

    // drive/head select
    // al = ((abs_lba >> 24) & 0x0F) | *((uint8_t*)drv + /* dd_sbits */ 4) | 0xE0;
    uint8_t devsel = 0xE0 | ((abs_lba >> 24) & 0x0F); // master
    outb(io_base + REG_DEVSEL, devsel);

    // outb(io_base + REG_DEVSEL, al);
    if(print_level > 1){print_string("devselect sent.\n\r");}

    // command
    outb(io_base + REG_COMMAND, 0x20);  // READ SECTORS
    if(print_level > 1){print_string("command sent.\n\r");}

    // wait for BSY clear and DRQ set
    do {
        al = inb(io_base + REG_STATUS);
        if (al & 0x1)
        {
            if(print_level > 0){print_string("err bit is set. exiting!\n\r");
            print_string("err id: ");
            char buffer[3];
            print_string(uint8_to_hex_prefixed(inb(ATA_PRIMARY_BASE + REG_ERROR), buffer));
            print_string("\n\r");}
            
            return;
        }
        if(print_level > 1){print_string("status: ");
        char buffer[3];
        print_string(uint8_to_hex_prefixed(al, buffer));
        print_string("\n\r");}
    } while ((al & 0x88) != 0x08);  // BSY=0, DRQ=1


    uint8_t *buf = (uint8_t *)dest;

    for (uint32_t s = 0; s < sectors; s++) {
        ata_wait_400ns(io_base);
        // wait for DRQ for THIS sector
        do {
            al = inb(io_base + REG_STATUS);
            if (al & 0x01) return;
        } while ((al & 0x88) != 0x08);
        for (int i = 0; i < 256; i++) {  // 256 words = 512 bytes
            uint16_t data;
            __asm__ volatile ("inw %1, %0"
                            : "=a"(data)
                            : "Nd"(io_base + REG_DATA));

            *buf++ = (uint8_t)(data & 0xFF);       // low byte
            *buf++ = (uint8_t)(data >> 8);         // high byte
        }
    }
}

void ata_wait_400ns(uint16_t io_base)
{
    inb(io_base + REG_STATUS); inb(io_base + REG_STATUS); inb(io_base + REG_STATUS); inb(io_base + REG_STATUS);
}