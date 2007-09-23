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

#include <kernel.h>
#include <semaphore.h>
#include <syscalls.h>
#include <assert.h>
#include <irq.h>
#include <interrupt.h>

/**
 * \brief Semaphore syscall definitions
 */
_syscall1(void, sem_down, semaphore_t*, s);
_syscall1(void, sem_up, semaphore_t*, s);
_syscall2(void, sem_init, semaphore_t*, s, int32_t, count);

void sys_sem_init(semaphore_t* s, int32_t count) {
	s->count = count;
	INIT_LIST_HEAD(&s->waiting);
}

void sys_sem_down(semaphore_t* s) {
	sched_lock();

	s->count--;

	// Assert on underflows
	sys_assert(s->count != INT32_MAX);

	if (s->count < 0)
		sys_block(&s->waiting);

	sched_unlock();
}


void sys_sem_up(semaphore_t* s) {
	sched_lock();
// 	assert(current->lock_count == 1);

	s->count++;

	// Assert on overflows
	sys_assert(s->count != INT32_MIN);
	
	if (s->count <= 0) {
		struct task_t* task;
		sys_assert(!list_isempty(&s->waiting));
		task = get_struct_task(list_get_front(&s->waiting));
		sys_unblock(task);
	}

	sched_unlock();
}
