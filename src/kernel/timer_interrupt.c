/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#define AOS_KERNEL_MODULE

#include <kernel.h>
#include <task.h>
#include <clock.h>
#include <timer.h>
#include <kernel.h>
#include <types.h>
#include <list.h>
#include <fragment.h>
#include <macros.h>

#include <arm/lpc2119.h>

void timer_interrupt(void) {
	uint32_t now;
	uint32_t elapsed_time;

	now = read_timer32();
	AOS_HOOK(timer_event,now);
	elapsed_time = uint32diff(now, read_timer32());
	elapsed_time = ciel(elapsed_time, UINT8_MAX);
	_aos_status.timer_hook_maxtime = max(elapsed_time, _aos_status.timer_hook_maxtime);
	do_context_switch = 1;
}

void led_irq_start(void);
void led_irq_end(void);


void sched(void) {
	struct task_t* next = NULL;
		
#ifdef SHARED_STACK
	/* Copy stack away from shared system stack */
	if (current) {
		uint32_t len = (REGISTER_TYPE)&__stack_usr_top__ - get_usermode_sp();
// 		void* dst = current->stack;
		void* src = (void*)&__stack_usr_top__ - len;
		
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
			if (!is_background())
				list_push_back(&readyQ,&current->q);
		}
	}
#endif

	if (list_isempty(&readyQ)) {
		next = idle_task; // Idle
	} else {
		next = get_struct_task(list_get_front(&readyQ));
		list_erase(&next->q);
	}
	
#ifdef SHARED_STACK
	/* Copy stack to shared stack */
	if (next) {
		uint32_t len = next->stack_size;
		void* dst = (void*)&__stack_usr_top__ - len;
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
		if (next->fragment) {
			load_fragment(dst,next->fragment);
			next->fragment = 0;
		}
		
	}
#endif
	
	next->state = RUNNING;

	current = next;
}


void process_wakeup(struct task_t* task) {
	struct list_head* insertion_point = NULL;
	struct list_head* e;

	task->state = READY;
	list_push_front(&readyQ , &task->q);

	do_context_switch = 1; // Signal context-switch

// 	list_push_back(&readyQ , &task->q);
	return;
	
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
		/*if (insertion_point) {
			if (ready_task->prio > INT8_MIN) ready_task->prio--;
			break;
		}*/

		// Equal-priority-tasks shold step in front of each other
		if (ready_task->prio >= task->prio) {
			insertion_point = e;
			break;
		}
	}

	if (insertion_point)
		list_push_front(insertion_point , &task->q);
	else {
		list_push_back(&readyQ , &task->q);
	}
	do_context_switch = 1; // Signal context-switch

}
