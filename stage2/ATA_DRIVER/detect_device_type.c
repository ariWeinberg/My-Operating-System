#include "ATA.h"

/* Returns: 
   >=0  -> device type (ATADEV_*)
   -1   -> error (sets ata_last_error) 
*/
int detect_devtype(int slavebit, DEVICE *ctrl)
{
    if (!ctrl || (slavebit != 0 && slavebit != 1))
        return ata_fail(INVALID_ARGUMENT);

    /* Soft reset and wait for drive ready */
    if (ata_soft_reset(ctrl->dev_ctl) < 0)
        return -1;

    /* select drive */
    outb(ctrl->base + REG_DEVSEL, 0xA0 | (slavebit << 4));

    /* wait 400ns for drive select to propagate */
    inb(ctrl->dev_ctl);
    inb(ctrl->dev_ctl);
    inb(ctrl->dev_ctl);
    inb(ctrl->dev_ctl);

    /* check for non-existent device */
    unsigned cl = inb(ctrl->base + REG_CYL_LO);
    unsigned ch = inb(ctrl->base + REG_CYL_HI);

    if (cl == 0xFF && ch == 0xFF)
        return ata_fail(DEVICE_DOES_NOT_EXIST);

    /* differentiate ATA, ATAPI, SATA and SATAPI */
    if (cl == 0x14 && ch == 0xEB) return ATADEV_PATAPI;
    if (cl == 0x69 && ch == 0x96) return ATADEV_SATAPI;
    if (cl == 0x00 && ch == 0x00) return ATADEV_PATA;
    if (cl == 0x3C && ch == 0xC3) return ATADEV_SATA;

    return ATADEV_UNKNOWN;
}
