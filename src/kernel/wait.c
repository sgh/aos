#include <wait.h>
#include <aos.h>


void cond_init(struct condition* cond) {
	INIT_LIST_HEAD(&cond->waiting);
	mutex_init(&cond->lock);
}

void cond_wait(struct condition* cond, uint32_t timeout) {
	/** @todo implement timeout- func */
	block(&cond->waiting);
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
