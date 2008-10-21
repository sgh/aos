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
#ifndef MUTEX_H
#define MUTEX_H

#include <aos/list.h>
#include <aos/errno.h>

#ifdef __cplusplus
	extern "C" {
#endif

struct task_t; // Forward declaration of struct task_t

#if defined (__arm__)
#ifdef __cplusplus
	#define DECLARE_MUTEX_LOCKED(m) mutex_t m = { lock : 1, waiting : LIST_HEAD_INIT(m.waiting), owner : (task_t* )0 }
	#define DECLARE_MUTEX_UNLOCKED(m) mutex_t m = { lock : 0, waiting : LIST_HEAD_INIT(m.waiting), owner : (task_t*)0 }
#else

	#define DECLARE_MUTEX_LOCKED(m) mutex_t m = { .lock = 1, .waiting = LIST_HEAD_INIT(m.waiting) }
	#define DECLARE_MUTEX_UNLOCKED(m) mutex_t m = { .lock = 0, .waiting = LIST_HEAD_INIT(m.waiting) }
#endif
/**
 * \brief A mutex
 */
typedef struct {
	uint8_t lock;								/**< \brief The actual mutex lock */
	struct list_head waiting;		/**< \brief List of waiting processes */
	struct task_t* owner;				/**< \brief The owner of this mutex */
} mutex_t;

#else
	#include <pthread.h>
	#include <time.h>

/**
 *  * \brief A mutex
 *   */
typedef struct {
  pthread_mutex_t lock;
  pthread_cond_t cond;
  int cnt;
} mutex_t;

#define DECLARE_MUTEX_UNLOCKED(m) mutex_t m = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER,1}
#define DECLARE_MUTEX_LOCKED(m) mutex_t m = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER,0}

#endif

/**
 * \brief Lock mutex
 * @param m The mutex to lock
 */
void mutex_lock(mutex_t* m);

/**
 * \brief Lock mutex with a timeout
 * @param m The mutex to lock
 * @param timeout Time to wait for lock in milliseconds
 * @return ESUCCESS or ETIMEOUT
 */
uint8_t mutex_timeout_lock(mutex_t* m, uint32_t timeoutms);


/**
 * \brief Unlock mutex
 * @param m The mutex to unlock
 */
void mutex_unlock(mutex_t* m);

/**
 * \brief Try to lock mutex
 * @param m The mutex to unlock
 * @return 0 if mutex is allready locked. 1 if mutex lock was succesfully acquired.
 */
uint8_t mutex_trylock(mutex_t* m);

/**
 * \brief Initialize mutex to unlocked
 * @param m The mutex to initialize
 */
void mutex_init(mutex_t* m);

#ifdef __cplusplus
	}
#endif

#endif
