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
#include <interrupt.h>
#include <string.h>

static void sched_switch(void);

void sched_clock(void) {

	current->ticks++;

	/*
		Check If current process has time left to run. If its time is up change
		context if any processes in the readyQ
	*/
	if (!current->time_left) {

		if (!list_isempty(&readyQ) /*&& current->prio > -127*/)
			current->resched = 1;

	} else // Very important to avoid underflow of time_left member
		current->time_left--;
}

void sched_lock(void) {
	current->lock_count++;
}

void sched_unlock(void) {
	uint32_t stat;

	/*
	 * Reentrancy handled by allways doing lock/unlock in pairs. The topmost
	 * interrupt is framed inside such a pair. As the lock_count is only
	 * decremented AFTER the sched_switch(), the interrupt-enabled part of
	 * sched_switch will still have lock_count > 0. The sched_switch will
	 * therefore only be executed once.
	 */
	interrupt_save(&stat);
	interrupt_disable();
	
	assert(current->lock_count > 0);

	if (current->lock_count == 1) {
		if (current->resched) {
			sched_switch(); // ZZZzzz
			current->resched = 0;
		}
	}
	current->lock_count--;

	interrupt_restore(stat);
}

/**
 * \brief Assembler function for switching context
 * @param old The context to switch from
 * @param  new The context to switch to
 */
void switch_context(struct context* old, struct context* new);

/**
 * \brief Switch to another process
 */
static FLATTEN void sched_switch(void) {
	struct task_t* prev = current;
	struct task_t* next = NULL;
	uint32_t stat;
	uint32_t time_longest;

/** @todo optimize this func. It is run with irq_lock held */
	
	// Find next process
	if (!list_isempty(&readyQ)) {
		next = get_struct_task(list_get_front(&readyQ));
		list_erase(&next->q);
	}

	// If process is preempted
	if (current->state == RUNNING) {
		current->state = READY;
		if (!is_background()) {

			/**
			 * Processed with time left to run is place en front of the queue. Other
			 * processes are placed last.
			 */
			if (current->time_left)
				list_push_front(&readyQ, &current->q);
			else
				list_push_back(&readyQ, &current->q);
		}
		current->nonvoluntary_ctxt++;
	} else
		current->voluntary_ctxt++;

	/** @todo this will eventually end up with QUANTUM as highes time_slice */
	time_longest = QUANTUM - current->time_left;
	if (time_longest > current->time_longest)
		current->time_longest = time_longest;

	if (!next)
		next = &idle_task;

	if (next->time_left == 0)
		next->time_left = QUANTUM;

	if (prev == next)
		return;

	// tell mm-system not to use scheck_lock
	mm_schedlock(0);
		
	// Enable interrupts
	interrupt_save(&stat);
	interrupt_enable();

	current->stack_size = (uint32_t)&__stack_usr_top__ - current->ctx.uregs->sp;

	if (current->stack_size > current->max_stack_size) {
		if (current->fragment) {
			free_fragment(current->fragment);
			current->fragment = NULL;
		}
		current->max_stack_size = current->stack_size;
	}

	if (!current->fragment)
		current->fragment = create_fragment(current->max_stack_size);

	store_fragment(current->fragment, (void*)current->ctx.uregs->sp, current->stack_size);

	if (next->fragment)
		load_fragment(&__stack_usr_top__ - next->stack_size, next->fragment);

	// Disable interrupts again
	interrupt_restore(stat);

	// Tell mm-system to use schedlock
	mm_schedlock(1);

	next->state = RUNNING;
	current = next;

	switch_context(&prev->ctx, &next->ctx); // zzzZZZ
}


void process_wakeup(struct task_t* task) {
	struct list_head* it;

	// Higher priority task is placed in front of tje queue
	if (task->prio < current->prio) {
		list_push_front(&readyQ , &task->q);
		current->resched = 1;
	} else { // Place according to priority
		struct list_head* insertion_point = NULL;
		list_for_each(it, &readyQ) {
			struct task_t* _t = get_struct_task(it);

			if (task->prio < _t->prio) {
				insertion_point = it;
				break;
			}
		}
		list_push_back(insertion_point ? insertion_point: &readyQ , &task->q);
	}

	task->state = READY;

	// Stop the timeout timer if it is active
	timer_stop(&task->timeout_timer);
}


void sched_sleep(uint16_t ms) {
	sched_lock();
	timer_timeout(&current->sleep_timer, (void*) process_wakeup, current, ms2ticks(ms));
	
	current->state = SLEEPING;
	current->resched = 1;
	sched_unlock();
}
