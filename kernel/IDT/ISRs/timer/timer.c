#include "timer.h"
#include "../../../PIC/PIC.h"
#include "../../../tasks/tasks.h"
#include "../../IDT.h"
#include "../../../STD/bool.h"
#include "timer.h"


volatile uint32_t k_ticks = 0;
volatile bool need_schedule = false;
volatile uint32_t pending_resume_eip = 0;

void timer_handler(void)
{
    k_ticks ++;
    scheduler_tick();
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
        "je .Ltimer_no_schedule\n\t"
        "movl 36(%%esp), %%eax\n\t"
        "movl %%eax, pending_resume_eip\n\t"
        "movl $.Ltimer_scheduler_resume, 36(%%esp)\n\t"
        "movl $0, need_schedule\n\t"

        ".Ltimer_no_schedule:\n\t"
        "popa\n\t"
        "add $4, %%esp\n\t"
        "iret\n\t"

        ".Ltimer_scheduler_resume:\n\t"
        "cli\n\t"
        "cmpl $0, critical\n\t"
        "jg .Ltimer_resume_original\n\t"
        "cmpl $2, task_count\n\t"
        "jl .Ltimer_resume_original\n\t"

        "call choose_next_task\n\t"
        "testl %%eax, %%eax\n\t"
        "jz .Ltimer_resume_original\n\t"
        "movl %%eax, %%edx\n\t"

        "pushl pending_resume_eip\n\t"
        "pushl %%edx\n\t"
        "pushl $.Ltimer_scheduler_cleanup\n\t"
        "jmp context_switch\n\t"

        ".Ltimer_scheduler_cleanup:\n\t"
        "addl $4, %%esp\n\t"
        "sti\n\t"
        "ret\n\t"

        ".Ltimer_resume_original:\n\t"
        "sti\n\t"
        "jmp *pending_resume_eip\n\t"
        :
        :
        : "eax", "edx", "memory"
    );
}
