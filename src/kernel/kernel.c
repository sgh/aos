#include <kernel.h>
#include <arm/lpc2119.h>
#include <fragment.h>

LIST_HEAD(readyQ);
LIST_HEAD(msleepQ);
LIST_HEAD(usleepQ);

uint8 do_context_switch = 0; /**< \brief Shall we do proccess-shift */


void free(void* segment);
void* malloc(unsigned short size);
	
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
		
		// DMEM
// 		current->malloc_stack = malloc(len);
// 		memcpy( current->malloc_stack, src, len);
		
		// Static mem
//  		memcpy( dst, src, len);
		
		// Fragmem
		current->fragment = store_fragment(src,len);
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
		
		// DMEM
// 		if (current->malloc_stack) {
// 			memcpy( dst, current->malloc_stack, len);
// 			free(current->malloc_stack);
// 			current->malloc_stack = 0;
// 		}
		
		// Static mem
// 		memcpy( dst, src, len);
		
		// Fragmem
		if (current->fragment) {
			load_fragment(dst,current->fragment);
			current->fragment = 0;
		}
		
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
	list_erase(&current->q);
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
