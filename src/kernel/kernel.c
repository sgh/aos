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

#include <macros.h>
#include <string.h>
#include <kernel.h>
#include <task.h>
#include <list.h>
#include <timer.h>
#include <clock.h>
#include <bits.h>
#include <irq.h>

LIST_HEAD(readyQ);
LIST_HEAD(usleepQ);


uint8_t do_context_switch = 0;

/**
 * \brief Is context-switches allowed.
 */
uint8_t allow_context_switch = 1;

/**
 * \brief Are interrupts disabled.
 */
// uint8_t interrupts_disabled = 0;


struct aos_hooks* _aos_hooks = NULL;
struct aos_status _aos_status;

struct task_t* idle_task;

/**
 * \brief Offset of struct task_t::contex
 * Used in assembler routines to fetch location of
 * the memory to store registers in.
 */
const uint32_t context_offset = offsetof(struct task_t,context);

// uint32_t last_context_time = 0;

struct task_t* current = NULL;

// Linker provides theese
extern funcPtr __initcalls_start__[];
extern funcPtr __initcalls_end__[];

// uint32_t get_interrupt_elapsed() {
// 	register uint32_t now = read_timer32();
// 	return now>=last_interrupt_time ? now-last_interrupt_time: UINT32_MAX - (last_interrupt_time-now);
// }

static void do_initcalls(void) {
	funcPtr* initcall;

	// Do initcalls 
	initcall = __initcalls_start__;
	while (initcall != __initcalls_end__) {
		(*initcall)();
		initcall++;
	}

}

void aos_basic_init() {
	interrupt_init();
	do_initcalls();
	current = NULL;
}


void aos_context_init(uint32_t timer_refclk) {
// 	int i;
	/** @todo this should problably be a syscall too */
	idle_task = create_task(NULL, NULL, 0);
	list_erase(&idle_task->q);
	current = idle_task;
	init_clock(timer_refclk);
	enable_clock();
}


void sys_yield(void) {
	do_context_switch = 1;
}



/**
 * \brief Check if current process is the idle-process.
 * If so it should not be allowed to block in any way
 * @return 0 if the current process is not the idle-process
 */
uint8_t is_background(void) {
	return (current == idle_task);
}

// static uint32_t time_slice_elapsed(void) {
	/** @todo  there might be a problem with uint32diff 
	that makes use in usleep leave some processes dead */
// 	return uint32diff(last_context_time, read_timer32());
// }

void sys_get_sysutime(uint32_t* time) {
	if (time == NULL)
		return;
	
	*time = ticks2us(system_ticks);
}

void sys_get_sysmtime(uint32_t* time) {
	if (time == NULL)
		return;
	
	*time = ticks2us(system_ticks);
}

void sys_aos_hooks(struct aos_hooks* hooks) {
	_aos_hooks = hooks;	
}


void sys_usleep(uint32_t us) {
	timer_timeout(&current->sleep_timer, process_wakeup, current, us2ticks(us));

	current->state = SLEEPING;
	do_context_switch = 1;
}

void sys_msleep(uint16_t ms) {
	timer_timeout(&current->sleep_timer, process_wakeup, current, ms2ticks(ms));

	current->state = SLEEPING;
	do_context_switch = 1;
}


void sys_block(struct list_head* q) {
	list_push_back(q,&current->q);
	current->state = BLOCKED;
	do_context_switch = 1;
}


void sys_unblock(struct task_t* task) {	
	if (task->state == BLOCKED ) {
		struct task_t* next = get_struct_task(list_get_front(&readyQ));
		process_wakeup(task);
		if (task->prio < next->prio)
			do_context_switch = 1;
	}
}


void sys_disable_cs() {
	allow_context_switch = 0;
}


void sys_enable_cs() {
	allow_context_switch = 1;
}


// void sys_disable_irqs() {
// 	interrupts_disabled = 1;
// }


// void sys_enable_irqs() {
// 	interrupts_disabled = 0;
// }
