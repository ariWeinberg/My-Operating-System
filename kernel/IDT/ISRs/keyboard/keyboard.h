#include "../../../STD/int.h"
#include "../../../UTILS/asm_utils.h" // inb/outb

#define KBD_PORT 0x60
#define PIC1_CMD 0x20


extern uint8_t scancode_buffer[1000];
extern char char_buffer[500];
extern uint32_t current_scancode_in;
extern uint32_t current_scancode_out;
extern uint32_t current_char_in;
extern uint32_t current_char_out;


void init_keyboard_irq();
void key_map_task();