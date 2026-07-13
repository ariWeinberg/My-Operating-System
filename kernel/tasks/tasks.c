#include "tasks.h"
#include "../IDT/IDT.h"
#include "../STD/defs.h"
#include "../STD/bool.h"
#include "../IDT/ISRs/timer/timer.h"

Task t_null = {0};
tasks_settings settings;

Task *task_list;
uint16_t current_index = 0;
uint16_t task_count = 0;
uint16_t task_slots = 0;
uint32_t next_task_id = 1;

uint32_t critical = 0;

bool is_initialized = false;

// Pointer to the task currently owning the CPU
Task* volatile running_task;

static bool ensure_task_capacity(void)
{
    if (task_count < task_slots)
    {
        return true;
    }

    uint16_t new_slots = task_slots + 1;
    Task *new_list = realloc(task_list, (uint16_t)(new_slots * sizeof(Task)));
    if (new_list == NULL)
    {
        return false;
    }

    task_list = new_list;
    task_slots = new_slots;
    return true;
}


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

Task *choose_next_task(void)
{
    if (!is_initialized || task_list == NULL || task_count < 2 || critical > 0)
    {
        return NULL;
    }

    uint16_t previous_index = current_index;
    for (uint16_t offset = 1; offset < task_count; offset++)
    {
        uint16_t candidate_index = (uint16_t)((previous_index + offset) % task_count);
        Task *candidate = &task_list[candidate_index];
        if (candidate->state != TASK_READY)
        {
            continue;
        }

        if (task_list[previous_index].state == TASK_RUNNING)
        {
            task_list[previous_index].state = TASK_READY;
        }
        candidate->state = TASK_RUNNING;
        current_index = candidate_index;
        return candidate;
    }

    return NULL;
}

void task_starter(void (*func)()) {
    __asm__ volatile("sti"); // Crucial: Re-enable interrupts for the new task
    func();
    scheduler_exit();
}

void init_tasks(void)
{
    settings.default_stack_size = 4096;

    if (!ensure_task_capacity())
    {
        return;
    }

    Task *kernel_task = &task_list[0];
    *kernel_task = (Task){0};
    kernel_task->id = 0;
    kernel_task->state = TASK_RUNNING;
    running_task = kernel_task;
    current_index = 0;
    task_count = 1;

    is_initialized = true;
}

Task k_create_task(void (*func)())
{
    if (!is_initialized || func == NULL)
    {
        return t_null;
    }

    uint32_t raw = (uint32_t)malloc(settings.default_stack_size + 32);
    if (raw == 0)
    {
        return t_null;
    }

    uint32_t stack_top = (raw + (uint32_t)settings.default_stack_size) & ~0xF;
    uint32_t *stack = (uint32_t *)(uintptr_t)stack_top;

    *(--stack) = (uint32_t)func;         // Argument for task_starter
    *(--stack) = 0;                      // Dummy return address
    *(--stack) = (uint32_t)task_starter; // context_switch returns to here
    
    for(int i=0; i<8; i++) *(--stack) = 0; // pushal frame

    Task t = {0};
    t.esp = (uint32_t)stack;
    t.stack_base = raw;
    t.stack_size = settings.default_stack_size;
    t.state = TASK_READY;
    t.id = next_task_id++;
    t.wake_tick = 0;

    return t;
}

void k_register_task(Task t)
{
    if (!is_initialized || t.esp == 0)
    {
        return;
    }

    enter_critical();
    if (!ensure_task_capacity())
    {
        exit_critical();
        return;
    }
    if (running_task != NULL)
    {
        running_task = &task_list[current_index];
    }
    task_list[task_count] = t;
    task_count++;
    exit_critical();
}

void k_create_and_register_task(void (*func)())
{
    Task t = k_create_task(func);
    if (t.esp == 0)
    {
        while (1); // later replace with a panic or an handle...
    }
    k_register_task(t);
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


void scheduler_yield()
{
    Task *next = choose_next_task();
    if (next != NULL)
    {
        context_switch(next);
    }
}

void scheduler_sleep(uint32_t ticks)
{
    if (running_task == NULL)
    {
        return;
    }

    interrupts_disable();
    running_task->wake_tick = k_ticks + ticks;
    running_task->state = TASK_SLEEPING;
    Task *next = choose_next_task();
    interrupts_enable();

    if (next != NULL)
    {
        context_switch(next);
    }
    else
    {
        running_task->state = TASK_RUNNING;
    }
}

void scheduler_block(uint32_t task_id)
{
    for (uint16_t i = 0; i < task_count; i++)
    {
        if (task_list[i].id != task_id)
        {
            continue;
        }

        task_list[i].state = TASK_BLOCKED;
        if (&task_list[i] == running_task)
        {
            scheduler_yield();
        }
        return;
    }
}

void scheduler_unblock(uint32_t task_id)
{
    for (uint16_t i = 0; i < task_count; i++)
    {
        if (task_list[i].id == task_id && task_list[i].state == TASK_BLOCKED)
        {
            task_list[i].state = TASK_READY;
            return;
        }
    }
}

void scheduler_exit()
{
    if (running_task == NULL)
    {
        return;
    }

    running_task->state = TASK_ZOMBIE;
    Task *next = choose_next_task();
    if (next != NULL)
    {
        context_switch(next);
    }

    while (1)
    {
        asm volatile("hlt");
    }
}

void scheduler_tick(void)
{
    for (uint16_t i = 0; i < task_count; i++)
    {
        Task *task = &task_list[i];
        if (task->state == TASK_SLEEPING && k_ticks >= task->wake_tick)
        {
            task->state = TASK_READY;
        }
    }
}
