#include <kernel.h>
#include <arm/lpc2119.h>

LIST_HEAD(readyQ);
LIST_HEAD(msleepQ);
LIST_HEAD(usleepQ);

uint8 do_context_switch = 0; /**< \brief Shall we do proccess-shift */


void sched(void) {
	if (current && current->state == RUNNING) {
		current->state = READY;
		list_push_back(&readyQ,&current->q);
	}
	
#ifdef SHARED_STACK
	/* Copy stack away from shared system stack */
	if (current) {
		uint32 len = (REGISTER_TYPE)&Top_Stack - get_usermode_sp();
		void* dst = current->stack;
		void* src = (void*)&Top_Stack - len;
		memcpy( dst, src, len);
		current->stack_size = len;
	}
#endif
	
	current = get_struct_task(list_get_front(&readyQ));
	list_erase(/*&readyQ,*/ &current->q);
	
#ifdef SHARED_STACK
	/* Copy stack to shared stack */
	if (current) {
		uint32 len = current->stack_size;
		void* dst = (void*)&Top_Stack - len;
		void* src = current->stack;
		memcpy( dst, src, len);
	}
#endif
	
	current->state = RUNNING;
}


void sys_yield(void) {
	do_context_switch = 1;
}


void sys_msleep(uint16 ms) {	
	current->sleep_time = ms*1000 + T1_TC;
// 	list_erase(&readyQ,&current->q);
	list_push_back(&msleepQ,&current->q);

	current->state = BLOCKED;
	do_context_switch = 1;
}


void sys_usleep(uint16 us) {
	current->sleep_time = us + T1_TC;
// 	list_erase(&readyQ,&current->q);
	list_push_front(&msleepQ,&current->q);

	current->state = BLOCKED;
	do_context_switch = 1;
}


void sys_block(struct list_head* q) {
	list_push_back(q,&current->q);
	current->state = BLOCKED;
	do_context_switch = 1;
}


void sys_unblock(struct task_t* task) {	
	if (task->state == BLOCKED ) {
		task->state = READY;
		list_push_front(&readyQ, &task->q);
		do_context_switch = 1;
	}
}
