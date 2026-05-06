#include "../STD/int.h"
#include "PIC.h"
#include "../UTILS/asm_utils.h" // for outb, inb if you already have it

// #define PIC1_CMD 0x20
// #define PIC1_DATA 0x21
// #define PIC2_CMD 0xA0
// #define PIC2_DATA 0xA1

// void pic_remap() {
//     uint8_t a1, a2;

//     a1 = inb(PIC1_DATA); // save masks
//     a2 = inb(PIC2_DATA);

//     outb(PIC1_CMD, 0x11); // start init
//     outb(PIC2_CMD, 0x11);

//     outb(PIC1_DATA, 0x20); // IRQ0–7 mapped to 0x20–0x27
//     outb(PIC2_DATA, 0x28); // IRQ8–15 mapped to 0x28–0x2F

//     outb(PIC1_DATA, 4);    // tell master about slave at IRQ2
//     outb(PIC2_DATA, 2);    // tell slave its cascade identity

//     outb(PIC1_DATA, 0x01); // 8086 mode
//     outb(PIC2_DATA, 0x01);

//     outb(PIC1_DATA, a1);   // restore masks
//     outb(PIC2_DATA, a2);
// }





void pic_send_eoi(uint8_t irq)
{
	if(irq >= 8)
		outb(PIC2_COMMAND,PIC_EOI);
	
	outb(PIC1_COMMAND,PIC_EOI);
}

void pic_disable(void) {
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}

void pic_remap(int offset1, int offset2)
{
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 1 << CASCADE_IRQ);        // ICW3: tell Master PIC that there is a slave PIC at IRQ2
	io_wait();
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
	
	outb(PIC1_DATA, ICW4_8086);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

	// Unmask both PICs.
	outb(PIC1_DATA, 0);
	outb(PIC2_DATA, 0);
}

void irq_mask_all(void)
{
    /* mask everything except keyboard if you want */
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
}

void irq_clear_all(void)
{
    /* mask everything except keyboard if you want */
    outb(0x21, 0x00);
    outb(0xA1, 0x00);
}


void irq_set_mask(uint8_t irq_line) {
    uint16_t port;
    uint8_t value;

    if(irq_line < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_line -= 8;
    }
    value = inb(port) | (1 << irq_line);
    outb(port, value);        
}

void irq_clear_mask(uint8_t irq_line) {
    uint16_t port;
    uint8_t value;

    if(irq_line < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_line -= 8;
    }
    value = inb(port) & ~(1 << irq_line);
    outb(port, value);        
}

static uint16_t __pic_get_irq_reg(int ocw3)
{
    /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
     * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
    outb(PIC1_COMMAND, ocw3);
    outb(PIC2_COMMAND, ocw3);
    return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

uint16_t pic_get_irr(void)
{
    return __pic_get_irq_reg(PIC_READ_IRR);
}

uint16_t pic_get_isr(void)
{
    return __pic_get_irq_reg(PIC_READ_ISR);
}