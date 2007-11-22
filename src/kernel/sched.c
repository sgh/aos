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

		if (!list_isempty(&readyQ))
			current->resched = 1;

	} else // Very important to avoid underflow of time_left member
		current->time_left--;
}

void sched_lock(void) {
	current->lock_count++;
}

void sched_unlock(void) {
	uint32_t stat;

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
static void sched_switch(void) {
	struct task_t* prev = current;
	struct task_t* next = NULL;
	uint32_t stat;

	// If processes is preempted
	if (current->state == RUNNING) {
		current->state = READY;
		if (!is_background())
			list_push_back(&readyQ, &current->q);
	}

	if (list_isempty(&readyQ)) {
		next = &idle_task; // Idle
	} else {
		next = get_struct_task(list_get_front(&readyQ));
		list_erase(&next->q);
	}

	if (prev == next)
		return;

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

	interrupt_restore(stat);

	next->state = RUNNING;
	current = next;

	switch_context(&prev->ctx, &next->ctx); // zzzZZZ
}


void process_wakeup(struct task_t* task) {

	/** @todo implement better schedueling which does not make often sleeping processes take all cpu-time */
	if (task->state == SLEEPING)
		list_push_front(&readyQ , &task->q);
	else
		list_push_back(&readyQ , &task->q);

	task->state = READY;

	current->resched = 1;

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
