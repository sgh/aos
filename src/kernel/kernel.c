#include <kernel.h>
#include <arm/lpc2119.h>
#include <fragment.h>

LIST_HEAD(readyQ);
LIST_HEAD(msleepQ);
LIST_HEAD(usleepQ);

uint8_t do_context_switch = 0; /**< \brief Shall we do proccess-shift. */
uint8_t allow_context_switch = 1; /**< \brief Is context-switches allowed. */

void free(void* segment);
void* malloc(unsigned short size);
	
void sched(void) {
#ifdef SHARED_STACK
	/* Copy stack away from shared system stack */
	if (current) {
		uint32_t len = (REGISTER_TYPE)&Top_Stack - get_usermode_sp();
// 		void* dst = current->stack;
		void* src = (void*)&Top_Stack - len;
		
		// DMEM
// 		current->malloc_stack = malloc(len);
// 		memcpy( current->malloc_stack, src, len);
		
		// Static mem
//  		memcpy( dst, src, len);
		
		// Fragmem
		current->fragment = store_fragment(src,len);
		if ((current->fragment == NULL) && (len > 0)) // This indicates OOM
			current->state = CRASHED;
		current->stack_size = len;
		
		if (current->state == RUNNING) {
			current->state = READY;
			list_push_back(&readyQ,&current->q);
		}
	}
#endif
	
	current = get_struct_task(list_get_front(&readyQ));
	list_erase(/*&readyQ,*/ &current->q);
	
#ifdef SHARED_STACK
	/* Copy stack to shared stack */
	if (current) {
		uint32_t len = current->stack_size;
		void* dst = (void*)&Top_Stack - len;
// 		void* src = current->stack;
		
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


void sys_msleep(uint16_t ms) {
	sys_usleep(ms*1000);
}


void sys_usleep(uint32_t us) {
	struct list_head* e;
	struct list_head* insertion_point = NULL;
	uint32_t time = us + T1_TC;
	
	// TODO: Implement busywait here if delay is smaller than interrupt latency.
	if (us == 0)
		return;

	/* Run through alle sleeping processes all decrement the time our current
	processs wants to sleep. If a longer-sleeping process i reached, the
	current process should be interted before that process.
	*/
	list_for_each(e,&msleepQ) {
		struct task_t* t = NULL;
		t = get_struct_task(e);
		if (time > t->sleep_time)
			time -= t->sleep_time;
		else {
			t->sleep_time -= time;
			insertion_point = e;
		}
	}

	current->sleep_time = time;

	if (insertion_point == NULL) {
		list_push_back(&msleepQ,&current->q);
	} else {
		list_push_back(insertion_point,&current->q);
	}

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


void sys_disable_cs() {
	allow_context_switch = 0;
}


void sys_enable_cs() {
	allow_context_switch = 1;
}