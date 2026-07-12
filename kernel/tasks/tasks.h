#ifndef TASKS
#define TASKS

#include "../STD/int.h"
#include "task.h"

__attribute__((naked)) void context_switch(Task* next_task);
void schedule_next_task(void);
void task_starter(void (*func)());
void k_create_task(void (*func)());

#endif