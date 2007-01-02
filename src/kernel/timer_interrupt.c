#define AOS_KERNEL_MODULE

#include <kernel.h>
#include <task.h>
#include <timer_interrupt.h>
#include <timer.h>
#include <kernel.h>
#include <types.h>
#include <list.h>
#include <fragment.h>
#include <macros.h>


void timer_interrupt_routine() {
	struct task_t* t;
	struct list_head* e;
	uint32_t now;
	uint32_t elapsed_time;
	uint32_t time_to_wake = MAX_TIME_SLICE_US;

	
	now = read_timer32();
	AOS_HOOK(timer_event,now);
	elapsed_time = uint32diff(last_interrupt_time, now);
	elapsed_time = ciel(elapsed_time, UINT8_MAX);
	_aos_status.timer_hook_maxtime = max(elapsed_time, _aos_status.timer_hook_maxtime);

	now = read_timer32();
	elapsed_time = uint32diff(last_interrupt_time, now);

	last_interrupt_time = read_timer32();

	// If a one process is waiting, do context_switch
	if (!list_isempty(&readyQ))
		do_context_switch = 1; // Signal context-switch
	
	// If someone is sleeping
	if (!list_isempty(&usleepQ)) {
		e = list_get_front(&usleepQ);
		t = get_struct_task(e);

		if (t->sleep_time) { // If process had time left to sleep
			if (t->sleep_time > elapsed_time)
				t->sleep_time -= elapsed_time;
			else
				t->sleep_time = 0;
		}

		if (t->sleep_time == 0) { // If process now has no time left to sleep
// 			struct task_t* next = get_struct_task(list_get_front(&readyQ));
			list_erase(&t->q);
			process_ready(t);
		}

		/*
		   Enable dynamic time-slice length. This is problematic since periodic function-calls
		   usd for regulators and such will not be called at regular intervals. This is why the following
		   is commented out.
		*/
// 		if (!list_isempty(&usleepQ)) {
// 			e = list_get_front(&usleepQ); // Set e to the next to wake up
// 			t = get_struct_task(e); // Get task-struct.
// 
// 			time_to_wake = t->sleep_time;
// 			if (time_to_wake < MIN_TIME_SLICE_US) // Make sure that timeslice stays above MIN_TIME_SLICE_US
// 				time_to_wake = MIN_TIME_SLICE_US;
// 
// 			if (time_to_wake > MAX_TIME_SLICE_US) // Make sure that timeslice stays below MAX_TIME_SLICE_US
// 				time_to_wake = MAX_TIME_SLICE_US;
// 		}

	}

	set_timer_match( get_timer_match() + time_to_wake );
	clear_timer_interrupt();
}


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
		if ((current->fragment == NULL) && (len > 0)) { // This indicates Stack-Alloc-Error
			current->state = CRASHED;
			AOS_HOOK(stack_alloc_fatal, current);
		}
		
		current->stack_size = len;
		
		if (current->state == RUNNING) {
			current->state = READY;
			list_push_back(&readyQ,&current->q);
		}
	}
#endif

	if (list_isempty(&readyQ))
		current = idle_task; // Idle
	else {
		current = get_struct_task(list_get_front(&readyQ));
		list_erase(/*&readyQ,*/ &current->q);
	}
	
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


void process_ready(struct task_t* task) {
	struct list_head* insertion_point = NULL;
	struct list_head* e;

	// Reset the age of the process.
	task->prio = task->prio_initial;
	task->state = READY;
	
	/*
	Run through the list to insert the task after higher priority-tasks.
	The process is inserted before the first process with a lower priority
	than the process itself. This way we can maintain an ordrered readyQ
	with aging implemented as task->priority_age += 1 if a process steps in front of it
	*/
	list_for_each(e,&readyQ) {
		struct task_t* ready_task;
		ready_task = get_struct_task(e);

		// If place of insertion, then the current procees must be the process right after.
		// Increment its age and break;
		if (insertion_point) {
			if (ready_task->prio > INT8_MIN) ready_task->prio--;
			break;
		}

		// Equal-priority-tasks shold not step in font of each other
		if (ready_task->prio > task->prio) {
			insertion_point = e;
		}
	}

	if (insertion_point)
		list_push_front(insertion_point , &task->q);
	else
		list_push_back(&readyQ , &task->q);
}
