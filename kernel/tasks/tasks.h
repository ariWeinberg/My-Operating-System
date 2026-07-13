#ifndef TASKS
#define TASKS

#include "../STD/int.h"
#include "task.h"
#include "../STD/defs.h"

__attribute__((naked)) void context_switch(Task* next_task);
void task_starter(void (*func)());
void k_create_and_register_task(void (*func)());
void enter_critical(void);
void exit_critical(void);
Task *choose_next_task(void);
void init_tasks(void);

void k_register_task(Task t);
Task k_create_task(void (*func)());

void scheduler_yield();
void scheduler_sleep(uint32_t ticks);
void scheduler_block(uint32_t task_id);
void scheduler_unblock(uint32_t task_id);
void scheduler_exit();
void scheduler_tick(void);

#endif
