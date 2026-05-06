#include "../../../UTILS/utils.h" // inb/outb
#include "../../../STD/defs.h"
#include "../../../SCREEN_DRIVER/screen_driver.h"
#include "./keyboard.h"


#define KBD_PORT 0x60
#define PIC1_CMD 0x20


uint8_t scancode_buffer[1000];
char char_buffer[500];
uint32_t current_scancode_in = 0;
uint32_t current_scancode_out = 0;
uint32_t current_char_in = 0;
uint32_t current_char_out = 0;


// size                              1     2      3       4       5       6       7       8       9       10      11      12     13      14
uint8_t scancode_map_indexes[]  = { 0x29,  0x02,  0x03,   0x04,   0x05,   0x06,   0x07,   0x08,   0x09,   0x0A,   0x0B,  0x0C,   0x0D,   0x0E,
//                                  15     16     17     18      19     20      21      22      23      24      25      26      27      28
                                    0x0F,  0x10,  0x11,  0x12,  0x13,   0x14,   0x15,   0x16,   0x17,   0x18,   0x19,   0x1A,   0x1B,   0x2B,
//                                  29     30     31     32      33      34      35      36      37      38      39      40
                                    0x1E,  0x1F,  0x20,  0x21,   0x22,   0x23,   0x24,   0x25,   0x26,   0x27,   0x28,   0x1C,
//                                  41     42     43      44      45      46      47      48      49     50
                                    0x2C,  0x2D,  0x2E,  0x2F,   0x30,   0x31,   0x32,   0x33,   0x34,   0x35,
//                                  51  
                                    0x39,
//                                  52    53    54    55    56    57    58    59    60    61    62    63    64    65    66    67    68    69    
                                    0x37, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x39, 0x39, 0x39, 0x39, 0x39, };
                                    
// size                             1      2      3       4       5       6       7       8       9       10      11      12      13      14
char    scancode_map_values[]   = { '`',   '1',   '2',   '3',    '4',    '5',    '6',    '7',    '8',    '9',    '0',    '-',    '=',    '\b',   
//                                  15     16     17     18      19      20      21      22      23      24      25      26      27      28
                                    '\t',  'q',   'w',   'e',    'r',    't',    'y',    'u',    'i',    'o',    'p',    '[',    ']',    '\\',
//                                  28     29     30      31      32      33      34      35      36      37      38     40
                                    'a',   's',   'd',    'f',    'g',    'h',    'j',    'k',    'l',    ';',    '\'',   '\n',
//                                  41     42      43     44      45      46      47      48      49      50
                                    'z',   'x',   'c',    'v',    'b',    'n',    'm',    ',',    '.',    '/',
//                                  51  
                                    ' ',
//                                  52    53    54    55    56    57    58    59    60    61    62    63    64    65    66    67    68    69  
                                    '*',  '7',  '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',  '2',  '3',  '0',  '.',  ' ',  ' ',  ' ',  ' ',
                                };
uint16_t map_size = 64;

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
