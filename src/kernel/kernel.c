#include <string.h>
#include <kernel.h>
#include <task.h>
#include <list.h>
#include <macros.h>
#include <timer_interrupt.h>
#include <bits.h>

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
uint8_t interrupts_disabled = 0;


struct aos_condition_handlers* condition_handlers = NULL;

struct task_t* idle_task;

/**
 * \brief Offset of struct task_t::contex
 * Used in assembler routines to fetch location of
 * the memory to store registers in.
 */
const uint32_t context_offset = offsetof(struct task_t,context);

uint32_t last_interrupt_time = 0;

struct task_t* current = NULL;

// Linker provides theese
extern funcPtr __initcalls_start__[];
extern funcPtr __initcalls_end__[];

uint32_t get_interrupt_elapsed() {
	register uint32_t now = read_timer();
	return now>=last_interrupt_time ? now-last_interrupt_time: 0xFFFFFFFF - (last_interrupt_time-now);
}

static void do_initcalls() {
	funcPtr* initcall;
	
	// Do initcalls 
	initcall = __initcalls_start__;
	while (initcall != __initcalls_end__) {
		(*initcall)();
		initcall++;
	}

}

void sys_aos_basic_init() {
	do_initcalls();
	current = NULL;
}


void aos_context_init(uint32_t timer_refclk, funcPtr idle_func) {
	idle_task = create_task(idle_func, 0);
	list_erase(&idle_task->q);
	init_timer_interrupt(timer_interrupt, timer_refclk);
	enable_timer_interrupt();
	yield();
}


void sys_yield(void) {
	do_context_switch = 1;
}


void sys_msleep(uint16_t ms) {
	sys_usleep(ms*1000);
}


static uint8_t is_background() {
	return (current == idle_task);
}

uint32_t time_slice_elapsed() {
	return uint32diff(last_interrupt_time, read_timer());
}

void sys_get_systime(uint32_t* time) {
	if (time != NULL)
		*time = read_timer();
}

void sys_usleep(uint32_t us) {
	struct list_head* e;
	struct list_head* insertion_point = NULL;
	uint32_t time = us + time_slice_elapsed();

	if (is_background())
		return;
	
	// TODO: Implement busywait here if delay is smaller than the minimum time-slice
// 	if (us < MIN_TIME_SLICE_US) {
// 		uint32_t target_time = read_timer();
// 		sys_get_systime(&target_time);
// 		while (
// 	}

	/* Run through alle sleeping processes all decrement the time our current
	processs wants to sleep. If a longer-sleeping process is reached, the
	current process should be interted before that process.
	*/
	list_for_each(e,&usleepQ) {
		struct task_t* t;
		t = get_struct_task(e);
		if (time > t->sleep_time)
			time -= t->sleep_time;
		else {
			t->sleep_time -= time;
			insertion_point = e;
			break;
		}
	}

	current->sleep_time = time;

	if (insertion_point == NULL)
		list_push_back(&usleepQ,&current->q);
	else
		list_push_back(insertion_point,&current->q);

	current->state = BLOCKED;
	do_context_switch = 1;
}

void sys_block(struct list_head* q) {
	if (current == idle_task)
		return;
	list_erase(&current->q);
	list_push_back(q,&current->q);
	current->state = BLOCKED;
	do_context_switch = 1;
}


void sys_unblock(struct task_t* task) {	
	if (task->state == BLOCKED ) {
		struct task_t* next = get_struct_task(list_get_front(&readyQ));
		process_ready(task);
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


void sys_disable_irqs() {
	interrupts_disabled = 1;
}


void sys_enable_irqs() {
	interrupts_disabled = 0;
}
