#include "../../UTILS/asm_utils.h" // inb/outb
#include "../../UTILS/utils.h" // inb/outb
#include "../../STD/defs.h"
#include "../IDT.h"
#include "../../SCREEN_DRIVER/screen_driver.h"

#define KBD_PORT 0x60
#define PIC1_CMD 0x20
uint8_t scancode_map_indexes[]  = {  0x02,  0x03,   0x04,   0x05,   0x06,   0x07,   0x08,   0x09,   0x0A,   0x0B,   0x0C,   0x0D,   0x0E,   0x0F,   };
char    scancode_map_values[]   = {  '1',   '2',   '3',    '4',    '5',    '6',    '7',    '8',    '9',    '0',    '-',    '=',    '\b',   '\t',    };
uint16_t map_size = 14;

char scancode_to_char(uint8_t scancode)
{
    if (scancode > 0x80)
    return '\0';
    for (int i = 0; i < map_size; i++)
    {
        if (scancode_map_indexes[i] == scancode)
        {
            return scancode_map_values[i];
        }
    }
    return '?';
}

// simple C handler for keyboard scan codes
void keyboard_handler_c(uint8_t scancode) {
    // TODO: convert to char or push to buffer
    // (void)scancode; // placeholder
    scancode = inb(0x60);

    put_char(scancode_to_char(scancode),0x0F);
    // print_string("keyboard event!\n");
}

// assembly ISR wrapper
__attribute__((naked)) void keyboard_isr(void)
{
    __asm__ volatile (
        "cli\n\t"
        "pusha\n\t"

        // read scancode
        "inb $0x60, %al\n\t"
        "movzbl %al, %eax\n\t"   // zero-extend to 32-bit
        "pushl %eax\n\t"
        "call keyboard_handler_c\n\t"
        "addl $4, %esp\n\t"

        "popa\n\t"

        // send EOI to PIC
        "movb $0x20, %al\n\t"
        "outb %al, $0x20\n\t"

        "sti\n\t"
        "iret\n\t"
    );
}



void init_keyboard_irq() {
    // keyboard is IRQ1 -> IDT entry 0x21 after PIC remap
    set_idt_entry(0x21, (uint32_t)keyboard_isr, 0x08, 0x8E);
}
