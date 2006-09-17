#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <types.h>
#include <atomic.h>

typedef uint32_t spinlock_t;

void spinlock_lock(spinlock_t* lock);
void spinlock_unlock(spinlock_t* lock);

#endif
