/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#define AOS_KERNEL_MODULE

#include <aos/macros.h>
#include <string.h>
#include <aos/kernel.h>
#include <aos/task.h>
#include <aos/list.h>
#include <aos/timer.h>
#include <aos/clock.h>
#include <aos/bits.h>
#include <aos/irq.h>
#include <aos/aos_hooks.h>
#include <aos/assert.h>
#include <aos/interrupt.h>

LIST_HEAD(readyQ);
// LIST_HEAD(usleepQ);
LIST_HEAD(process_list);


uint32_t num_context_switch;


struct aos_hooks* _aos_hooks = NULL;
struct aos_status _aos_status;

struct task_t idle_task;
uint8_t default_preemptive = 1;


/**
 * \brief Offset of struct task_t::contex
 * Used in assembler routines to fetch location of
 * the memory to store registers in.
 */
// const uint32_t context_offset = offsetof(struct task_t,context);

// uint32_t last_context_time = 0;

struct task_t* current = NULL;

// Linker provides theese
extern funcPtr __initcalls_start__[];
extern funcPtr __initcalls_end__[];
extern funcPtr __aos_sot__; // AOS Start-Of-Text
extern funcPtr __aos_eot__; // AOS End-Of-Text

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
	irq_init();
// 	init_runtime_check();
}


void aos_context_init(uint32_t timer_refclk) {
	// Setup idle task
	init_task(&idle_task, NULL, NULL, 127);
	idle_task.preemptive = 1;
	idle_task.name = "Idle";
	list_push_back(&process_list, &idle_task.glist);

	current = &idle_task;
	current->state = RUNNING;

	sched_lock(); // Lock scheduler

	do_initcalls();

	init_clock(timer_refclk);
	enable_clock();
	
	sched_unlock(); // Unlock scheduler
}


void sys_yield(void) {
	sched_lock();
	
	// Only do context-switch if someone else want CPU-time
	if (!list_isempty(&readyQ)) {
		if (!is_background()) {
			current->resched = 1;
			current->state = READY;
			list_push_back(&readyQ, &current->q);
		}
	}

	sched_unlock();
}

void sys_aos_default_preemptive(uint8_t preemptive) {
	default_preemptive =  preemptive ? 1 : 0;
}

void sys_aos_set_preemptive(uint8_t preemptive) {
	sched_lock();
	current->preemptive = preemptive ? 1 : 0;
	sched_unlock();
}


// static uint32_t time_slice_elapsed(void) {
	/** @todo  there might be a problem with uint32diff 
	that makes use in usleep leave some processes dead */
// 	return uint32diff(last_context_time, read_timer32());
// }

/*void sys_get_sysutime(uint32_t* time) {
	if (time == NULL)
		return;
	
	*time = ticks2us(system_ticks);
}*/

void /*sys_*/get_sysmtime(uint32_t* time) {
	if (time == NULL)
		return;
	
	*time = ticks2ms(system_ticks);
}

void sys_aos_hooks(struct aos_hooks* hooks) {
	_aos_hooks = hooks;	
}

void sys_msleep(uint16_t ms) {
// 	assert(current->lock_count == 1);

	sched_sleep(ms);
}

struct list_head* sys_get_process_list(void) {
  return ( &process_list ); 
}

void sys_block(struct list_head* q) {
// 	irq_lock();
	
	list_push_back(q, &current->q);
	current->resched = 1;
	current->state = BLOCKED;

// 	irq_unlock();
}


void sys_unblock(struct task_t* task) {
// 	irq_lock();
	
	sys_assert(task->state == BLOCKED);
	list_erase(&task->q);
	process_wakeup(task);

// 	irq_unlock();
}


void add_task_to_readyQ(struct task_t* task) {
	uint_fast8_t inserted = 0;
	struct list_head* it;

	task->state = READY;
	list_for_each(it, &readyQ) {
		struct task_t* _t = get_struct_task(it);

		if (task->prio < _t->prio) {
			inserted = 1;
			list_push_back(it, &task->q);
			break;
		}
	}
	if (!inserted)
		list_push_back(&readyQ, &task->q);
}


struct task_t* sys_create_task(taskFuncPtr entrypoint, const char* name, void* arg, int8_t priority) {
	struct task_t* t;
	t = sys_malloc(sizeof(struct task_t));
	init_task(t, entrypoint, arg, priority);
	t->name = name;
	add_task_to_readyQ(t);
	list_push_back(&process_list, &t->glist);
	return t;
}

void sys_delete_task(struct task_t* t) {
	sched_lock();
	list_erase(&t->glist);
	list_erase(&t->q);
	sched_unlock();
	destroy_task(t);
	free(t);
}


//void validate_execution_address(uint32_t address) {
//	ASSERT(address >= (uint32_t)&__aos_sot__);
//	ASSERT(address <= (uint32_t)&__aos_eot__);
//}


// void sys_disable_irqs() {
// 	interrupts_disabled = 1;
// }


// void sys_enable_irqs() {
// 	interrupts_disabled = 0;
// }
