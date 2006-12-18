#ifndef MUTEX_H
#define MUTEX_H

#include <list.h>

/**
 * \brief A mutex
 */
typedef struct {
	uint32_t spinlock;					/**< \brief The spinlock to use */
	uint8_t lock;								/**< \brief The actual mutex lock */
	struct list_head waiting;		/**< \brief List of waiting processes */
} mutex_t;

/**
 * \brief Lock mutex
 * @param m The mutex to lock
 */
void mutex_lock(mutex_t* m);

/**
 * \brief Unlock mutex
 * @param m The mutex to unlock
 */
void mutex_unlock(mutex_t* m);

/**
 * \brief Initialize mutex to unlocked
 * @param m The mutex to initialize
 */
void mutex_init(mutex_t* m);

#endif
