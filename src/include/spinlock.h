#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <types.h>
#include <atomic.h>

typedef uint32_t spinlock_t;

/**
 * \brief Lock a spinlock
 * @param lock The spinlcock to lock
 */
void spinlock_lock(spinlock_t* lock);

/**
 * \brief Unlock a spinlock
 * @param lock The spinlock to lock
 */
void spinlock_unlock(spinlock_t* lock);

#endif
