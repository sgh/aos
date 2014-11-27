/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#ifndef AOS_SEMAPHORE_H
#define AOS_SEMAPHORE_H

#include <aos/types.h>
#include <aos/mutex.h>
#include <aos/list.h>
#include <aos/macros.h>

#define DECLARE_SEMAPHORE(s,val) semaphore_t s = { .count = val, .waiting = LIST_HEAD_INIT(s.waiting) }

#ifdef __cplusplus
	extern "C" {
#endif

#if defined(__arm__)

/**
 * \brief A semaphore
 */
typedef struct {
	int32_t count;						/**< \brief The semaphore count */
	struct list_head waiting;	/**< \brief The list of waiting processes */
} semaphore_t;

void sem_upn(semaphore_t* s, uint32_t n);
uint32_t  sem_downn(semaphore_t* s, uint32_t n);

/**
 * \brief Initialize semaphore
 * @param s The semaphore to initialixe
 * @param count The semaphore-count to set
 */
void sem_init(semaphore_t* s, int32_t count);

#else

#include <pthread.h>
#include <time.h>

#define sem_init semaphore_init

/**
 *  * \brief A semaphore
 *   */
typedef struct {
  pthread_mutex_t lock;
  pthread_cond_t cond;
  int cnt;
} semaphore_t;

void semaphore_init(semaphore_t *m, uint32_t n);

#endif

/**
 * \brief Count up the semaphore. This may be called from interrupt-routines
 * @param s The semaphore to increment
 */
void sem_up(semaphore_t* s);

/**
 * \brief Count down the semaphore, and possibly block the process if count
 * is decremented below 0. THIS MAY NOT BE CALLED FROM INTERRUPT-ROUTINES
 * @param s The semaphore the descrement
 */
void sem_down(semaphore_t* s);

/**
 * \brief Count down the semaphore, return 1 if success, 0 if teh semaphore
 * is <=0. THIS MAY NOT BE CALLED FROM INTERRUPT-ROUTINES
 * @param s The semaphore the descrement
 */
uint8_t sem_trydown(semaphore_t* s);

/**
 * \brief Count down the semaphore, and possibly block the process if count
 * is decremented below 0. THIS MAY NOT BE CALLED FROM INTERRUPT-ROUTINES
 * @param m The mutex to lock
 * @param timeout Time to wait for down in milliseconds
 * @return ESUCCESS or ETIMEOUT
 */
uint8_t sem_timeout_down(semaphore_t* s, uint32_t timeoutms);



#if defined(__linux__)
static inline int32_t sem_upn( semaphore_t* s, int32_t count) {
  int32_t n = count;

  while(count--)
    sem_up(s);
  return n;
}

static inline int32_t sem_downn( semaphore_t* s, UNUSED int32_t count) {
  sem_down(s);
  return 1;
}

static inline void sys_sem_up( semaphore_t* s ) {
  sem_up(s);
}

static inline void sys_sem_down( semaphore_t* s ) {
  sem_down(s);
}
#endif


#ifdef __cplusplus
	}
#endif

#endif
