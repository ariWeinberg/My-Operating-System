#ifndef KEYBOARD
#define KEYBOARD
#include "../../IDT.h"
#include "../../../STD/int.h"
#include "../../../STD/bool.h"
#include "../../../UTILS/asm_utils.h" // inb/outb

#define KBD_PORT 0x60
#define PIC1_CMD 0x20

void handle_scancode(void);
void init_keyboard_irq(void);

bool keyboard_pop_char(char *out);

#endif
