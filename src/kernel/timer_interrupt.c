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
#include <irq.h>
#include <assert.h>

void sched(void);

void sched_clock(void) {
	AOS_HOOK(timer_event,ticks2ms(system_ticks));

	current->ticks++;

	/*
		Check If current process has time left to run. If its time is up change
		context if any processes in the readyQ
	*/
	if (!current->time_left) {

		irq_lock();
		if (!list_isempty(&readyQ))
			do_context_switch = 1;
		irq_unlock();

	} else // Very important to avoid underflow of time_left member
		current->time_left--;
}

void sched_lock(void) {
}

void sched_unlock(void) {
}


void sched(void) {
	static uint32_t ticks;
	static uint32_t last_task_switch = 0;
	struct task_t* next = NULL;
	

	/* Copy stack away from shared system stack */
	if (current) {
#ifdef SHARED_STACK
		uint32_t top_stack = (REGISTER_TYPE)&__stack_usr_top__;
		uint32_t sp = /*get_usermode_sp()*/current->context->sp;
		uint32_t len = top_stack - sp;
		void* src = (void*)&__stack_usr_top__ - len;
		
		sys_assert(sp <= top_stack);

		// If stack-size has increased, or no stack is pressent, (re)allocate the stack-fragment
 		if (len > current->stack_size) {
			if (current->fragment) {
				free_fragment(current->fragment);
				current->fragment = NULL;
			}

			//if (!current->fragment)
				current->fragment = create_fragment(len);
		}

		if (len > 0) {
			sys_assert(current->fragment);
		
			store_fragment(current->fragment, src, len);
		}

		current->stack_size = len;

		sys_assert(len == 0 || (current->fragment != NULL));  // This indicates Stack-Alloc-Error

		if (current->stack_size > current->max_stack_size)
			current->max_stack_size = current->stack_size;
#endif

		if (current->state == RUNNING) {
			current->state = READY;
			if (!is_background())
				list_push_back(&readyQ,&current->q);
		}

		// Compensate for context-switches between timer ticks. Without this
		// many tasks will use little or no time at all.
		current->subticks += (1000 - last_task_switch + get_clock()) % 1000;

		last_task_switch = get_clock();

		if (current->subticks >= 1000) {
			current->subticks -= 1000;
			current->ticks++;
		}

		check_stack();
	}

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

		// Fragmem
		if (next->fragment)
			load_fragment(dst,next->fragment);
		
	}
#endif
	
	next->state = RUNNING;

	sys_assert(current != next);
	
	current = next;

	ticks = current->ticks;

	current->time_left = ms2ticks(TIME_SLICE_MS);

	// Clear context-switch-flag
	do_context_switch = 0;

	// Maintain statistics
	num_context_switch++;
}

void process_wakeup(struct task_t* task) {
	struct list_head* insertion_point = NULL;
	struct list_head* e;

	if (task->state == SLEEPING)
		insertion_point = &readyQ;
		
	task->state = READY;
	/*
		Run through the list to insert the task after higher priority-tasks.
		The process is inserted before the first process with a lower priority.
	*/
	if (!insertion_point) {
		list_for_each(e,&readyQ) {
			struct task_t* ready_task;
			ready_task = get_struct_task(e);
	
			// Process may not step in front of equal-priority tasks
			if (ready_task->prio > task->prio) {
				insertion_point = e;
				break;
			}
		}
	}

	/*
		If insertionpoint is found 1 or more processe exist in the readyQ
	*/
	if (insertion_point) {
		/* Now, if process is inserted as the first in the readyQ, do context-switch */
		list_push_front(insertion_point , &task->q);
		if (insertion_point == &readyQ)
			do_context_switch = 1; // Signal context-switch
	} else { 
		/*	
			insertion_point==NULL => No processes in the readyQ
			Put in readyQ and do context-switch immediately
		*/
		list_push_front(&readyQ , &task->q);
		do_context_switch = 1; // Signal context-switch
	}


}
