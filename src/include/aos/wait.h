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
#ifndef AOS_WAIT_H
#define AOS_WAIT_H

#include <aos/mutex.h>
#include <aos/list.h>

/**
 * \brief Structure for waiting on an event
 */
struct condition {
	mutex_t lock;							/**< \brief Mutex for the waiting list */
	struct list_head waiting;	/**< \brief List of waiting processes */
};

/**
 * \brief Initialize af wait-condition
 * @param cond 
 */
void cond_init(struct condition* cond);


/**
 * \brief Wait for a condition
 * @param cond Pointer to the struct condition to wait for
 * @param timeout A timeout in ms to maximum wait
 * @return @todo how do we tell that the wait has been interrupted by a timeout
 */
uint8_t cond_wait(struct condition* cond, uint32_t timeout);


/**
 * \brief Signal the first process waiting on a condition to wake up
 * @param cond The struct condition to signal
 */
void cond_signal(struct condition* cond);


/**
 * \brief Signal all waiters on a given struct condition to wake up
 * @param cond The struct condition to signal
 */
void cond_broadcast(struct condition* cond);

#endif
