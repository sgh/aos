#include <semaphore.h>
#include <kernel.h>
#include <list.h>

/**
 * \brief Semaphore syscall definitions
 */
_syscall1(void,sem_down, struct semaphore_t*, s);
_syscall1(void,sem_up, struct semaphore_t*, s);


void sys_sem_down(struct semaphore_t* s) {
	s->counter--;
	
	if (s->counter < 0) {
		current->state = BLOCKED;
		list_erase(/*&readyQ,*/&current->q);
		list_push_back(&s->waiting, &current->q);
		do_context_switch = 1;
	}
}


void sys_sem_up(struct semaphore_t* s) {
	struct task_t* task;
	s->counter++;
		
	if (s->counter <= 0) {
		task = get_struct_task(list_get_front(&s->waiting));
		task->state = READY;
		list_push_front(&readyQ,&task->q);
		do_context_switch = 1;
	}
	
}
