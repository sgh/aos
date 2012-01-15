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

#include <aos/kernel.h>
#include <aos/task.h>
#include <aos/clock.h>
#include <aos/timer.h>
#include <aos/kernel.h>
#include <aos/types.h>
#include <aos/list.h>
#include <aos/fragment.h>
#include <aos/macros.h>
#include <aos/irq.h>
#include <aos/assert.h>
#include <aos/interrupt.h>
#include <string.h>

static FLATTEN void sched_switch(void);

void sched_clock(void) {

	current->ticks++;

	if (!current->preemptive)
		return;

	/*
		Check If current process has time left to run. If its time is up change
		context if any processes in the readyQ
	*/
	if (!current->time_left) {
		if (!list_isempty(&readyQ))
			current->resched = 1;
	} else // Very important to avoid underflow of time_left member
		current->time_left--;
}

void sched_lock(void) {
	current->lock_count++;
}

void HOT sched_unlock(void) {
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
	
	//assert(current->lock_count > 0);

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
static FLATTEN HOT void sched_switch(void) {
	struct task_t* prev = current;
	struct task_t* next = NULL;
	uint32_t time_longest;

/** @todo optimize this func. It is run with irq_lock held */

	// Find next proces
	if (!list_isempty(&readyQ)) {
		next = get_struct_task(list_get_front(&readyQ));
		list_erase(&next->q);
	}

	// If process is preempted
	if (prev->state == RUNNING) {
		if (!is_background()) {
			add_task_to_readyQ(prev);
			assert(current->preemptive==1);
		}
		prev->nonvoluntary_ctxt++;
	} else // Otherwise it is a voluntary context-switch
		prev->voluntary_ctxt++;

	/** @todo this will eventually end up with QUANTUM as highes time_slice */
	time_longest = QUANTUM - prev->time_left;
	if (time_longest > prev->time_longest)
		prev->time_longest = time_longest;

	if (!next)
		next = &idle_task;

	if (next->time_left == 0)
		next->time_left = (prev->prio < next->prio) ? 1 : QUANTUM;

	if (prev == next)
		return;

	// tell mm-system not to use scheck_lock
	mm_schedlock(0);

	// Enable interrupts
	interrupt_enable();

	prev->stack_size = (uint32_t)&__stack_usr_top__ - prev->ctx.uregs->sp;

	if (prev->stack_size > prev->max_stack_size) {
		if (prev->fragment) {
			free_fragment(prev->fragment);
			prev->fragment = NULL;
		}
		prev->max_stack_size = prev->stack_size;
	}

	if (!prev->fragment)
		prev->fragment = create_fragment(prev->max_stack_size);

	store_fragment(prev->fragment, (void*)prev->ctx.uregs->sp, prev->stack_size);

	if (next->fragment)
		load_fragment(&__stack_usr_top__ - next->stack_size, next->fragment);

        // Disable interrupts again
        interrupt_disable();

	// Tell mm-system to use schedlock
	mm_schedlock(1);

	next->state = RUNNING;
	current = next;
	
	num_context_switch++;
	switch_context(&prev->ctx, &next->ctx); // zzzZZZ
}


void process_wakeup(struct task_t* task) {
	// Resched if task is preemptive
	if (task->prio < current->prio)
		current->resched = current->preemptive;

	add_task_to_readyQ(task);

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
