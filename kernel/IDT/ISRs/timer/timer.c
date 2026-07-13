#include "timer.h"
#include "../../../PIC/PIC.h"
#include "../../../tasks/tasks.h"
#include "../../IDT.h"
#include "../../../STD/bool.h"
#include "timer.h"


volatile uint32_t k_ticks = 0;
volatile bool need_schedule = false;

void timer_handler(void)
{
    k_ticks ++;
    if (k_ticks % 2 == 0) {
        // schedule_next_task();
        need_schedule = true;
    }
}


void init_timer_irq(void)
{
    idt_set_interrupt_32(0x20, &timer_isr);
    irq_clear_mask(0); // timer IRQ
}

// __attribute__((naked))
// void timer_isr(void) {
//     asm volatile(
//         "pusha\n\t"

//         // Send EOI IMMEDIATELY so other interrupts (keyboard) can fire
//         "movb $0x20, %%al\n\t"
//         "outb %%al, $0x20\n\t"
//         "outb %%al, $0xA0\n\t"

//         "mov 32(%%esp), %%eax\n\t"
//         "push %%eax\n\t"
//         "call timer_handler\n\t"
//         "cmpl $0, need_schedule\n\t"
//         "je after_scedhuler\n\t"
//         "call schedule_next_task\n\t"
//         "after_scedhuler:\n\t"
//         "add $4, %%esp\n\t"

//         "popa\n\t"
//         "add $4, %%esp\n\t"
//         "iret\n\t"
//         : : : "memory"
//     );
// }

__attribute__((naked))
void timer_isr(void)
{
    asm volatile(
        "pusha\n\t"

        "movb $0x20, %%al\n\t"
        "outb %%al, $0x20\n\t"

        "mov 32(%%esp), %%eax\n\t"
        "push %%eax\n\t"
        "call timer_handler\n\t"
        "add $4, %%esp\n\t"

        "cmpl $0, need_schedule\n\t"
        "je 1f\n\t"
        "cmpl $0, critical\n\t"
        "jg 1f\n\t"
        "cmpl $2, task_count\n\t"
        "jl 1f\n\t"

        // Save the interrupted task's complete timer frame.
        "movl running_task, %%edx\n\t"
        "movl %%esp, (%%edx)\n\t"

        // Select the next task, wrapping at task_count.
        "movl current_index, %%eax\n\t"
        "incl %%eax\n\t"
        "cmpl task_count, %%eax\n\t"
        "jl 2f\n\t"
        "xorl %%eax, %%eax\n\t"
        "2:\n\t"
        "movl %%eax, current_index\n\t"

        // Load the next task's complete timer frame.
        "leal task_list(,%%eax,4), %%edx\n\t"
        "movl %%edx, running_task\n\t"
        "movl (%%edx), %%esp\n\t"
        "movl $0, need_schedule\n\t"

        "1:\n\t"
        "popa\n\t"
        "add $4, %%esp\n\t"
        "iret\n\t"
        :
        :
        : "eax", "edx", "memory"
    );
}
