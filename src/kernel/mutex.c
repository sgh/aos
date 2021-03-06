/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#define AOS_KERNEL_MODULE
// #include <aos.h>
#include <aos/kernel.h>
#include <aos/errno.h>

/**
 * \brief Mutex syscall definitions
 */
_syscall1(void, mutex_init, mutex_t*, m);
_syscall1(void, mutex_lock, mutex_t*, m);
_syscall2(uint8_t, mutex_timeout_lock, mutex_t*, m, uint32_t, timeoutms);
_syscall1(uint8_t, mutex_trylock, mutex_t*, m);
_syscall1(void, mutex_unlock, mutex_t*, m);


static void mutex_timeout(void* arg) {
	struct task_t* t = (struct task_t*)arg;

	sys_unblock(t);
	t->sleep_result = ETIMEOUT;
}


void sys_mutex_lock(mutex_t* m) {
	sys_mutex_timeout_lock(m, 0);
}


uint8_t sys_mutex_timeout_lock(mutex_t* m,  uint32_t timeoutms) {
	sched_lock();

	current->sleep_result = ESUCCESS; // Default we return ESUCCESS

	if (!m->lock) { // Mutex is not locked - lock it
		m->lock = 1;
		m->owner = current;
		sched_unlock();
		return current->sleep_result;
	}

	sys_block(&m->waiting);
	
	current->wait_mutex = m;
	
	// Setup timeout for lock timeout
	if (timeoutms > 0)
		timer_timeout(&current->timeout_timer, (void*) mutex_timeout, current, ms2ticks(timeoutms));

	sched_unlock();
	
	current->wait_mutex = NULL;

	// Return sleep_result. ETIMEOUT indicates a timeout
	return current->sleep_result;
}

uint8_t sys_mutex_trylock(mutex_t* m) {
	sched_lock();
	if (!m->lock) { // Mutex is not locked - lock it
		m->lock = 1;
		sched_unlock();
		return 1;
	}

	sched_unlock();
	return 0;
}

void sys_mutex_init(mutex_t* m) {
	INIT_LIST_HEAD(&m->waiting);
}

void sys_mutex_unlock(mutex_t* m) {
	struct task_t* next;

	sched_lock();
	if (list_isempty(&m->waiting)) { // If none is waiting
		m->lock = 0;
		m->owner = NULL;
		sched_unlock();
		return;
	}

	
	next = get_struct_task(list_get_front(&m->waiting));
	m->owner = next;

	sys_unblock(next);
	sched_unlock();
}

