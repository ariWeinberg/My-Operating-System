#ifndef TASK
#define TASK

#include "../STD/int.h"
#include "../MEMORY_MANAGMENT/memory_managment.h"

typedef enum {
    TASK_UNUSED,
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_SLEEPING,
    TASK_ZOMBIE
} task_state_t;

typedef struct task {
    uint32_t esp;
    uint32_t stack_base;
    uint32_t stack_size;
    uint32_t id;
    task_state_t state;
    uint32_t wake_tick;
} Task;

typedef struct tasks_settings
{
    uint64_t default_stack_size;
} tasks_settings;


// typedef struct {
//     uint32_t esp;
// } Task;

#endif
