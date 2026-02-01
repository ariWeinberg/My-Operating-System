#include "ATA.h"

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
