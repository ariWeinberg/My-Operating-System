#include "tasks.h"
#include "../IDT/IDT.h"
#include "../STD/defs.h"


Task task_list[6]; 
int current_index = 0;
int task_count = 1; 

uint32_t critical = 0;

// Pointer to the task currently owning the CPU
Task* volatile running_task = &task_list[0];

__attribute__((naked))
void context_switch(Task* next_task) {
    asm volatile(
        "pushal\n\t"              // 1. Save current task's state
        "movl running_task, %eax\n\t"
        "movl %esp, (%eax)\n\t"   // 2. Save current ESP into running_task->esp

        "movl 36(%esp), %eax\n\t" // 3. Get 'next_task' argument (offset 36 because of pushal + return address)
        "movl %eax, running_task\n\t" // 4. Update running_task pointer
        "movl (%eax), %esp\n\t"   // 5. Load the NEW task's ESP

        "popal\n\t"               // 6. Restore new task's state
        "iret\n\t"                 // 7. Jump to new task's saved EIP
    );
}

Task *choose_next_task(void)
{
    if (critical > 0)
    {
        return NULL;
    }
    int next_index = (current_index + 1) % task_count;
    if (next_index == current_index) return NULL; // Don't switch to yourself

    Task* next = &task_list[next_index];
    current_index = next_index;
    return next;
}

// static volatile void schedule_next_task(void) {
    
//     Task *next = choose_next_task();
//     context_switch(next);
// }


void task_starter(void (*func)()) {
    __asm__ volatile("sti"); // Crucial: Re-enable interrupts for the new task
    func();
    while(1); 
}


void k_create_task(void (*func)()) {
    if (task_count >= 6 || func == 0)
    {
        return;
    }

    uint32_t raw = (uint32_t)malloc(4096 + 32);
    if (raw == 0)
    {
        return;
    }

    uint32_t* stack = (uint32_t*)((raw + 4096) & ~0xF);

    *(--stack) = (uint32_t)func;         // task_starter argument
    *(--stack) = 0;                      // task_starter return address
    *(--stack) = 0x202;                  // EFLAGS: interrupts enabled
    *(--stack) = 0x08;                   // CS: kernel code segment
    *(--stack) = (uint32_t)task_starter; // initial EIP
    *(--stack) = 0x20;                   // timer IRQ stub ID

    for (int i = 0; i < 8; i++)
    {
        *(--stack) = 0;                  // pusha frame
    }

    task_list[task_count].esp = (uint32_t)stack;
    task_count++;
}

void enter_critical(void)
{
    if (!critical)
    {
        interrupts_disable();
    }
    critical++;
}

void exit_critical(void)
{
    if (critical > 0)
    {
        critical--;
    }
    if (!critical)
    {
        interrupts_enable();
    }
}
