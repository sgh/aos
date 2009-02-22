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
#include <aos/wait.h>
#include <aos/aos.h>
#include <aos/task.h>


void cond_init(struct condition* cond) {
	INIT_LIST_HEAD(&cond->waiting);
	mutex_init(&cond->lock);
}

uint8_t cond_wait(struct condition* cond, UNUSED uint32_t timeout) {
	/** @todo implement timeout- func */
	block(&cond->waiting);
	return 0;
}

void cond_signal(struct condition* cond){
	mutex_lock(&cond->lock);
	while (!list_isempty(&cond->waiting)) { // If none is waiting
		struct task_t* next;
		next = get_struct_task(list_get_front(&cond->waiting));
		list_erase(&next->q);
		unblock(next);
	}
	mutex_unlock(&cond->lock);
}

void cond_broadcast(struct condition* cond) {
	mutex_lock(&cond->lock);
	struct task_t* next;
	while (!list_isempty(&cond->waiting)) { // If none is waiting
		next = get_struct_task(list_get_front(&cond->waiting));
		list_erase(&next->q);
		unblock(next);
	}
	mutex_unlock(&cond->lock);
}
