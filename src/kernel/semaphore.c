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
#include <aos.h>

/**
 * \brief Semaphore syscall definitions
 */
// _syscall1(void,sem_down, struct semaphore_t*, s);
// _syscall1(void,sem_up, struct semaphore_t*, s);

void sem_init(semaphore_t* s, int16_t count) {
	s->count = count;
	mutex_init(&s->lock);
	INIT_LIST_HEAD(&s->waiting);
}

void sem_down(semaphore_t* s) {
	mutex_lock(&s->lock);
	s->count--;
	
	if (s->count < 0) {
		mutex_unlock(&s->lock);
		block(&s->waiting);
/*		current->state = BLOCKED;
		list_erase(&current->q);
		list_push_back(&s->waiting, &current->q);*/
	} else
		mutex_unlock(&s->lock);
}


void sem_up(semaphore_t* s) {
	mutex_lock(&s->lock);
	s->count++;
		
	if (s->count <= 0 && !list_isempty(&s->waiting)) {
		struct task_t* task;
		task = get_struct_task(list_get_front(&s->waiting));
		mutex_unlock(&s->lock);
		unblock(task);
	/*	task = get_struct_task(list_get_front(&s->waiting));
		task->state = READY;
		list_push_front(&readyQ,&task->q);*/
	} else
		mutex_unlock(&s->lock);
}
