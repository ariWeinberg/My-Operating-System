#include "../STD/defs.h"
#include "tasks.h"
#include "mutex.h"
#include "semaphore.h"

void mutex_init(Mutex *mutex)
{
    if (mutex == NULL)
        return;

    semaphore_init(&mutex->semaphore, 1);
    mutex->owner_id = 0;
}

void mutex_lock(Mutex *mutex)
{
    if (mutex == NULL || running_task == NULL)
        return;

    semaphore_wait(&mutex->semaphore);
    mutex->owner_id = running_task->id;
}

void mutex_unlock(Mutex *mutex)
{
    if (mutex == NULL || running_task == NULL)
        return;

    if (mutex->owner_id != running_task->id)
        return;
    
    mutex->owner_id = 0;
    semaphore_post(&mutex->semaphore);
}
