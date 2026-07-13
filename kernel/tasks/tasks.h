#ifndef TASKS
#define TASKS

#include "../STD/int.h"
#include "task.h"
#include "../STD/defs.h"

__attribute__((naked)) void context_switch(Task* next_task);
void task_starter(void (*func)());
void k_create_task(void (*func)());
static void schedule_next_task(void);
void enter_critical(void);
void exit_critical(void);
void context_switch(Task* next_task);
Task *choose_next_task(void);

static volatile void schedule_next_task(void) {
    
    Task *next = choose_next_task();
    if (next != NULL)
    {
        context_switch(next);
    }
}

#endif