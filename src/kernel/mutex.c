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

void sys_mutex_lock(mutex_t* m) {
// 	spinlock_lock(&m->spinlock);

	if (!m->lock) { // Mutex is not locked - lock it
		m->lock = 1;
// 		spinlock_unlock(&m->spinlock);
		return;
	}

// 	spinlock_unlock(&m->spinlock);
	
	sys_block(&m->waiting);

}

uint8_t sys_mutex_trylock(mutex_t* m) {
// 	spinlock_lock(&m->spinlock);

	if (!m->lock) { // Mutex is not locked - lock it
		m->lock = 1;
// 		spinlock_unlock(&m->spinlock);
		return 1;
	}
// 	spinlock_unlock(&m->spinlock);

	return 0;
}

void mutex_init(mutex_t* m) {
	INIT_LIST_HEAD(&m->waiting);
}

void sys_mutex_unlock(mutex_t* m) {
	struct task_t* next;
// 	spinlock_lock(&m->spinlock);
	if (list_isempty(&m->waiting)) { // If none is waiting
		m->lock = 0;
// 		spinlock_unlock(&m->spinlock);
		return;
	}

	next = get_struct_task(list_get_front(&m->waiting));
	list_erase(&next->q);
// 	spinlock_unlock(&m->spinlock);
	
	sys_unblock(next);
}

