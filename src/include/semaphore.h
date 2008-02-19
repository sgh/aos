/*
		AOS - ARM Operating System
		Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

		This library is free software; you can redistribute it and/or
		modify it under the terms of the GNU Lesser General Public
		License as published by the Free Software Foundation; either
		version 2.1 of the License, or (at your option) any later version.

		This library is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
		Lesser General Public License for more details.

		You should have received a copy of the GNU Lesser General Public
		License along with this library; if not, write to the Free Software
		Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
		*/
#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <types.h>
#include <mutex.h>
#include <list.h>

#define DECLARE_SEMAPHORE(s,val) semaphore_t s = { .count = val, .waiting = LIST_HEAD_INIT(s.waiting) }

/**
 * \brief A semaphore
 */
typedef struct {
	int32_t count;						/**< \brief The semaphore count */
	struct list_head waiting;	/**< \brief The list of waiting processes */
} semaphore_t;

/**
 * \brief Initialize semaphore
 * @param s The semaphore to initialixe
 * @param count The semaphore-count to set
 */
void sem_init(semaphore_t* s, int32_t count);

/**
 * \brief Count down the semaphore, and possibly block the process if count
 * is decremented below 0. THIS MAY NOT BE CALLED FROM INTERRUPT-ROUTINES
 * @param s The semaphore the descrement
 */
void sem_down(semaphore_t* s);

/**
 * \brief Count down the semaphore, and possibly block the process if count
 * is decremented below 0. THIS MAY NOT BE CALLED FROM INTERRUPT-ROUTINES
 * @param m The mutex to lock
 * @param timeout Time to wait for down in milliseconds
 * @return ESUCCESS or ETIMEOUT
 */
uint8_t sem_timeout_down(semaphore_t* s, uint32_t timeoutms);


/**
 * \brief Count up the semaphore. This may be called from interrupt-routines
 * @param s The semaphore to increment
 */
void sem_up(semaphore_t* s);

#endif
