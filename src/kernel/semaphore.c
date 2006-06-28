#include <semaphore.h>
#include <kernel.h>
#include <queue.h>

/**
 * \brief Semaphore syscall definitions
 */
_syscall1(void,sem_P, struct semaphore_t*, s);
_syscall1(void,sem_V, struct semaphore_t*, s);


void sys_sem_P(struct semaphore_t* s) {
	s->counter--;
	
	if (s->counter < 0) {
		current->state = BLOCKED;
		list_erase(&readyQ,&current->q);
		list_push_back(&s->waiting, &current->q);
		do_task_switch = 1;
	}
}


void sys_sem_V(struct semaphore_t* s) {
	struct task_t* task;
	s->counter++;
		
	if (s->counter <= 0) {
		task = get_struct_task(list_get_front(&s->waiting));
		task->state = READY;
		list_push_front(&readyQ,&task->q);
		do_task_switch = 1;
	}
	
}
