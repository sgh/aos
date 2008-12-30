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

#include <aos/kernel.h>
#include <aos/semaphore.h>
#include <aos/syscalls.h>
//#include <aos/assert.h>
#include <aos/irq.h>
#include <aos/interrupt.h>

/**
 * \brief Semaphore syscall definitions
 */
_syscall1(void, sem_down, semaphore_t*, s);
_syscall2(uint32_t, sem_downn, semaphore_t*, s, uint32_t, n);
_syscall1(uint8_t, sem_trydown, semaphore_t*, s);
_syscall1(void, sem_up, semaphore_t*, s);
_syscall2(void, sem_upn, semaphore_t*, s, uint32_t, n);
_syscall2(void, sem_init, semaphore_t*, s, int32_t, count);
_syscall2(uint8_t, sem_timeout_down, semaphore_t*, s, uint32_t, timeoutms);

void sys_sem_init(semaphore_t* s, int32_t count) {
	s->count = count;
	INIT_LIST_HEAD(&s->waiting);
}


static void semaphore_timeout(void* arg) {
	struct task_t* t = (struct task_t*)arg;

	t->sleep_result = ETIMEOUT;
	t->wait_semaphore->count++;

	sys_unblock(t);
}


uint8_t HOT sys_sem_timeout_down(semaphore_t* s, uint32_t timeoutms) {
	sched_lock();
	
	current->sleep_result = ESUCCESS; // Default we return EESUCCESS
	s->count--;

	// Assert on underflows
	//sys_assert(s->count != INT32_MAX);

	if (s->count < 0) {
		sys_block(&s->waiting);
		current->wait_semaphore = s;

		// Setup timeout for lock timeout
		if (unlikely(timeoutms > 0))
			timer_timeout(&current->timeout_timer, (void*) semaphore_timeout, current, ms2ticks(timeoutms));
		sched_unlock();
		current->wait_semaphore = NULL;
	} else
		sched_unlock();

	return current->sleep_result;
}


void sys_sem_down(semaphore_t* s) {
	sys_sem_timeout_down(s, 0);
}

uint32_t sys_sem_downn(semaphore_t* s, uint32_t n) {
	uint32_t timeoutms = 0;
	uint8_t blocknow = 1;
	uint32_t count = 1;

	if (n == 0)
		return 0;

	sched_lock();

	current->sleep_result = ESUCCESS; // Default we return EESUCCESS
	

	if (s->count > 0) {
		blocknow = 0;
		
		if (n > s->count) {
			count = s->count;
			s->count = 0;
		} else {
			count = n;
			s->count -= n;
		}
	} else
		s->count--;

	// Assert on underflows
	//sys_assert(s->count != INT32_MAX);

	if (blocknow) {
		sys_block(&s->waiting);
		current->wait_semaphore = s;
		// Setup timeout for lock timeout
		if (timeoutms > 0)
			timer_timeout(&current->timeout_timer, (void*) semaphore_timeout, current, ms2ticks(timeoutms));
	}

	sched_unlock();
	
	current->wait_semaphore = NULL;

	return count;
}

uint8_t sys_sem_trydown(semaphore_t* s) {
	sched_lock();

	if (s->count > 0) {
		s->count--;
		// Assert on underflows
		//sys_assert(s->count != INT32_MAX);

		sched_unlock();
		return 1;
	}

	sched_unlock();
	return 0;
}



void sys_sem_up(semaphore_t* s) {
	sched_lock();
 	//assert(current->lock_count == 1);

	s->count++;

	// Assert on overflows
	//sys_assert(s->count != INT32_MIN);
	
	if (s->count <= 0) {
		struct task_t* task;
		//sys_assert(!list_isempty(&s->waiting));
		task = get_struct_task(list_get_front(&s->waiting));
		sys_unblock(task);
	}

	sched_unlock();
}


void sys_sem_upn(semaphore_t* s, uint32_t n) {
	int32_t old;
	sched_lock();
	//assert(current->lock_count == 1);

	old = s->count;

	s->count += n;

	// Assert on overflows
	//sys_assert(s->count != INT32_MIN);
	
	while ((old < 0) && (n > 0)) {
		struct task_t* task;

		//sys_assert(!list_isempty(&s->waiting));
		task = get_struct_task(list_get_front(&s->waiting));
		sys_unblock(task);
		n--;
		old++;
	}

	sched_unlock();
}
