#ifndef SEMAPHORE
#define SEMAPHORE

#include "task.h"

typedef struct semaphore {
    uint32_t count;
    WaitQueue waiters;
} Semaphore;

void semaphore_init(Semaphore *semaphore, uint32_t initial_count);
void semaphore_wait(Semaphore *semaphore);
void semaphore_post(Semaphore *semaphore);

#endif