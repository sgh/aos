#include <semaphore.h>
#include <mutex.h>
#include <kernel.h>
#include <list.h>
#include <macros.h>

/**
 * \brief Semaphore syscall definitions
 */
// _syscall1(void,sem_down, struct semaphore_t*, s);
// _syscall1(void,sem_up, struct semaphore_t*, s);

void sem_init(struct semaphore_t* s, uint16_t count) {
	s->count = count;
	mutex_init(&s->lock);
}

void sem_down(struct semaphore_t* s) {
	mutex_lock(&s->lock);
	s->count--;
	
	if (s->count < 0) {
		current->state = BLOCKED;
		list_erase(/*&readyQ,*/&current->q);
		list_push_back(&s->waiting, &current->q);
	}
	mutex_unlock(&s->lock);
}


void sem_up(struct semaphore_t* s) {
	struct task_t* task;
	mutex_lock(&s->lock);
	s->count++;
		
	if (s->count <= 0) {
		task = get_struct_task(list_get_front(&s->waiting));
		task->state = READY;
		list_push_front(&readyQ,&task->q);
	}
	mutex_unlock(&s->lock);
}
