#include "timer.h"
#include "../../../PIC/PIC.h"
#include "../../../tasks/tasks.h"
#include "../../IDT.h"
#include "timer.h"


volatile uint32_t k_ticks = 0;

void timer_handler(void)
{
    k_ticks ++;
    if (k_ticks % 1 == 0) {
        schedule_next_task();
    }
}


void init_timer_irq(void)
{
    idt_set_interrupt_32(0x20, &timer_isr);
    irq_clear_mask(0); // timer IRQ
}
__attribute__((naked))
void timer_isr(void) {
    asm volatile(
        "pusha\n\t"

        // Send EOI IMMEDIATELY so other interrupts (keyboard) can fire
        "movb $0x20, %%al\n\t"
        "outb %%al, $0x20\n\t"
        "outb %%al, $0xA0\n\t"

        "mov 32(%%esp), %%eax\n\t"
        "push %%eax\n\t"
        "call timer_handler\n\t"
        "add $4, %%esp\n\t"

        "popa\n\t"
        "add $4, %%esp\n\t"
        "iret\n\t"
        : : : "memory"
    );
}
