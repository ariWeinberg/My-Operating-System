#include "semaphore.h"
#include "tasks.h"
#include "../IDT/IDT.h"


void semaphore_init(Semaphore *semaphore, uint32_t initial_count)
{
    if (semaphore == NULL)
        return;

    semaphore->count = initial_count;
    wait_queue_init(&semaphore->waiters);
}

void semaphore_wait(Semaphore *semaphore)
{
    if (semaphore == NULL)
        return;

    while (1)
    {
        interrupts_disable();

        if (semaphore->count > 0)
        {
            semaphore->count--;
            interrupts_enable();
            return;
        }

        scheduler_wait(&semaphore->waiters);
    }
}

void semaphore_post(Semaphore *semaphore)
{
    if (semaphore == NULL)
        return;

    interrupts_disable();
    semaphore->count++;
    
    if (semaphore->waiters.head != NULL)
    {
        scheduler_wake_one_locked(&semaphore->waiters);
    }

    interrupts_enable();
}
