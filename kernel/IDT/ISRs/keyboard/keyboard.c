#include "../../../UTILS/utils.h" // inb/outb
#include "../../../STD/defs.h"
#include "../../../STD/bool.h"
#include "../../../SCREEN_DRIVER/screen_driver.h"
#include "./keyboard.h"
#include "./scancode_map.h"
#include "./shifted_scancode_map.h"
#include "../../../PIC/PIC.h"
#include "../../../STD/buffers/ring_buffer/ring_buffer.h"


#define KBD_PORT 0x60
#define PIC1_CMD 0x20

bool is_l_shifted = false;

RingBuffer character_buffer;
RingBuffer scancode_buffer;

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

void handle_scancode(void)
{
    uint8_t scancode;
    if(ring_buffer_pop(&scancode_buffer, &scancode))
    {
        char character = scancode_to_char(scancode);
        ring_buffer_push(&character_buffer, character);
    }
}

// simple C handler for keyboard scan codes
void keyboard_handler_c(uint8_t scancode) {
    if (!ring_buffer_is_full(&scancode_buffer))
    {
        if (ring_buffer_push(&scancode_buffer, scancode))
        {
            handle_scancode();
        }
    }
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

void init_keyboard_irq(void) {
    ring_buffer_init(&character_buffer, 10000);
    ring_buffer_init(&scancode_buffer, 1024);
    // keyboard is IRQ1 -> IDT entry 0x21 after PIC remap
    idt_set_entry(0x21, (uint32_t)keyboard_isr, 0x08, 0x8E, 0);
    irq_clear_mask(1);
}

bool keyboard_pop_char(char *out)
{
    if (out == NULL)
        return false;

    interrupts_disable();
    bool result = ring_buffer_pop(&character_buffer, (uint8_t *)out);
    interrupts_enable();

    return result;
}
