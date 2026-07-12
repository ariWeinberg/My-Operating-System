#include "../../../UTILS/utils.h" // inb/outb
#include "../../../STD/defs.h"
#include "../../../STD/bool.h"
#include "../../../SCREEN_DRIVER/screen_driver.h"
#include "./keyboard.h"
#include "./scancode_map.h"
#include "./shifted_scancode_map.h"


#define KBD_PORT 0x60
#define PIC1_CMD 0x20


uint8_t scancode_buffer[1000];
char char_buffer[500];
uint32_t current_scancode_in = 0;
uint32_t current_scancode_out = 0;
uint32_t current_char_in = 0;
uint32_t current_char_out = 0;

bool is_l_shifted = false;

char scancode_to_char(uint8_t scancode)
{
    if (scancode == 0x2A)
    {
        is_l_shifted = true;
        return '\0';
    }
    if (scancode == 0xAA)
    {
        is_l_shifted = false;
        return '\0';
    }
    if (scancode > 0x80)
    return '\0';
    if (is_l_shifted)
    {
        for (int i = 0; i < map_size; i++)
        {
            if (shifted_scancode_map_indexes[i] == scancode)
            {
                return shifted_scancode_map_values[i];
            }
        }
    }
    
    for (int i = 0; i < map_size; i++)
    {
        if (scancode_map_indexes[i] == scancode)
        {
            return scancode_map_values[i];
        }
    }
    return '?';
}

void key_map_task()
{
    while (1)
    {
        handle_scancode(' ');
    }
}
void handle_scancode(uint8_t scancode)
{
    char buf[5];
    // print_string("in KB handller\n");
    char_buffer[current_char_in++] = scancode_to_char(scancode);
    // print_string(uint8_to_hex_prefixed(scancode, buf));
    // put_char('\n',0x0F);
                // put_char(char_buffer[current_char_out++], 0x0F);
    // put_char('\n',0x0F);
}

// simple C handler for keyboard scan codes
void keyboard_handler_c(uint8_t scancode) {
    scancode = inb(0x60);
    if (current_scancode_in < 999)
    scancode_buffer[current_scancode_in++] = scancode;
        handle_scancode(scancode);

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
    idt_set_entry(0x21, (uint32_t)keyboard_isr, 0x08, 0x8E, 0);
}
