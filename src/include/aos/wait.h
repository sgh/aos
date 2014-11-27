/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
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
