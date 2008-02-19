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
#define AOS_KERNEL_MODULE
// #include <aos.h>
#include <kernel.h>
#include <errno.h>

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
	
	// Setup timeout for lock timeout
	if (timeoutms > 0)
		timer_timeout(&current->timeout_timer, (void*) mutex_timeout, current, ms2ticks(timeoutms));

	sched_unlock();

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

