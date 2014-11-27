/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
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
