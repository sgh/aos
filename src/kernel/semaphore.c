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
