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

/**
 * \brief Mutes syscall definitions
 */
_syscall1(void, mutex_init, mutex_t*, m);
_syscall1(void, mutex_lock, mutex_t*, m);
_syscall1(uint8_t, mutex_trylock, mutex_t*, m);
_syscall1(void, mutex_unlock, mutex_t*, m);

void sys_mutex_lock(mutex_t* m) {
	sched_lock();
	
	if (!m->lock) { // Mutex is not locked - lock it
		m->lock = 1;
		sched_unlock();
		return;
	}
	sys_block(&m->waiting);
	sched_unlock();

	/** @todo check if current == m->owner. If not we have reached a timeout and should return ETIMEOUT */
// 	return ESUCCESS;
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
		sched_unlock();
		return;
	}

	next = get_struct_task(list_get_front(&m->waiting));

	sys_unblock(next);
	sched_unlock();
}

