#include "ATA.h"
#include "../UTILS/asm_utils.h"
#include "../MEMORY_MANAGMENT/memory_managment.h"
#include "../UTILS/utils.h"
#include "../SCREEN_DRIVER/screen_driver.h"

DEVICE ata_primary = {
    .base    = ATA_PRIMARY_BASE,
    .dev_ctl = ATA_PRIMARY_DEVCTL
};

static void clear_HOB_bits(void);
static int test_floating(void);
static int test_if_drive_exists(void);

int ata_init(void)
{
    clear_HOB_bits();

    if (test_floating())
        return ata_fail(BUS_FLOAT);

    if (test_if_drive_exists())
        return ata_fail(DEVICE_DOES_NOT_EXIST);

    return ata_ok();
}

static int test_floating(void)
{
    uint8_t status = inb(ATA_PRIMARY_BASE + REG_STATUS);
    return (status == 0xFF);
}

static void clear_HOB_bits(void)
{
    uint8_t val = inb(ATA_PRIMARY_DEVCTL);
    val &= ~0x80;             // clear HOB bit
    outb(ATA_PRIMARY_DEVCTL, val);
}

static int test_if_drive_exists(void)
{
    /* write known pattern to test registers */
    outb(0x172, 0x8B);
    outb(0x173, 0x73);
    outb(0x174, 0x74);

    /* read back */
    if (inb(0x172) != 0x8B) return 1;
    if (inb(0x173) != 0x73) return 1;
    if (inb(0x174) != 0x74) return 1;

    return 0;  // drive exists
}
