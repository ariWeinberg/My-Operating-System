#ifndef TIMER_IRQ123
#define TIMER_IRQ123

#include "../../../STD/int.h"

extern volatile uint32_t k_ticks;

void timer_handler(void);
void init_timer_irq(void);
__attribute__((naked)) void timer_isr(void);

#endif
