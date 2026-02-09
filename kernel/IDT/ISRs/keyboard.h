#include "../../STD/int.h"
#include "../../UTILS/asm_utils.h" // inb/outb

#define KBD_PORT 0x60
#define PIC1_CMD 0x20

void init_keyboard_irq();
