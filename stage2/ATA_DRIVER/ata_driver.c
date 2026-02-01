
#include "ATA.h"
#include "../UTILS/asm_utils.h"
#include "../MEMORY_MANAGMENT/memory_managment.h"
#include "../UTILS/utils.h"
#include "../SCREEN_DRIVER/screen_driver.h"


DEVICE ata_primary = {
    .base    = ATA_PRIMARY_BASE,
    .dev_ctl = ATA_PRIMARY_DEVCTL
};

int test_floating();
void clear_HOB_bits();

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

int ata_init()
{
    clear_HOB_bits();

    if(test_floating())
    {
        set_ata_last_error(BUS_FLOAT);
        return -1;
    }

    if(test_if_drive_exists())
    {
        set_ata_last_error(DEVICE_DOES_NOT_EXIST);
        return -2;
    }
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
