#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <types.h>
#include <mutex.h>
#include <list.h>

/**
 * \brief A semaphore
 */
struct semaphore_t {
	int16_t count;						/**< \brief The semaphore count */
	mutex_t lock;							/**< \brief The mutex used to lock */
	struct list_head waiting;	/**< \brief The list of waiting processes */
};

/**
 * \brief Initialize semaphore
 * @param s The semaphore to initialixe
 * @param count The semaphore-count to set
 */
void sem_init(struct semaphore_t* s, uint16_t count);

/**
 * \brief Count down the semaphore, and possibly block the process if count
 * is decremented below 0. THIS MAY NOT BE CALLED FROM INTERRUPT-ROUTINES
 * @param s The semaphore the descrement
 */
void sem_down(struct semaphore_t* s);

/**
 * \brief Count up the semaphore. This may be called from interrupt-routines
 * @param s The semaphore to increment
 */
void sem_up(struct semaphore_t* s);

#endif
