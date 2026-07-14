#ifndef MUTEX
#define MUTEX

#include "semaphore.h"

typedef struct mutex {
    Semaphore semaphore;
    uint32_t owner_id;
} Mutex;

void mutex_init(Mutex *mutex);
void mutex_lock(Mutex *mutex);
void mutex_unlock(Mutex *mutex);

#endif