
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
int test_if_drive_exists();

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
