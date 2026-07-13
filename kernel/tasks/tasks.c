#include "tasks.h"


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
        "ret\n\t"                 // 7. Jump to new task's saved EIP
    );
}

void schedule_next_task(void) {
    if (critical > 0)
    {
        return;
    }
    int next_index = (current_index + 1) % task_count;
    if (next_index == current_index) return; // Don't switch to yourself

    Task* next = &task_list[next_index];
    current_index = next_index;
    
    context_switch(next);
}


void task_starter(void (*func)()) {
    __asm__ volatile("sti"); // Crucial: Re-enable interrupts for the new task
    func();
    while(1); 
}


void k_create_task(void (*func)()) {
    uint32_t raw = (uint32_t)malloc(4096 + 32);
    uint32_t* stack = (uint32_t*)((raw + 4096) & ~0xF);

    *(--stack) = (uint32_t)func;         // Argument for task_starter
    *(--stack) = 0;                      // Dummy return address
    *(--stack) = (uint32_t)task_starter; // context_switch returns to here
    
    for(int i=0; i<8; i++) *(--stack) = 0; // pushal frame

    task_list[task_count].esp = (uint32_t)stack;
    task_count++;
}

void enter_critical(void)
{
    critical++;
}

void exit_critical(void)
{
    if (critical > 0)
    {
        critical--;
    }
}
