#define AOS_KERNEL_MODULE

#include <macros.h>
#include <string.h>
#include <kernel.h>
#include <task.h>
#include <list.h>
#include <timer.h>
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


struct aos_hooks* _aos_hooks = NULL;
struct aos_status _aos_status;

struct task_t* idle_task;

/**
 * \brief Offset of struct task_t::contex
 * Used in assembler routines to fetch location of
 * the memory to store registers in.
 */
const uint32_t context_offset = offsetof(struct task_t,context);

uint32_t last_context_time = 0;

struct task_t* current = NULL;

// Linker provides theese
extern funcPtr __initcalls_start__[];
extern funcPtr __initcalls_end__[];

// uint32_t get_interrupt_elapsed() {
// 	register uint32_t now = read_timer32();
// 	return now>=last_interrupt_time ? now-last_interrupt_time: UINT32_MAX - (last_interrupt_time-now);
// }

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
	/** @todo this should problably be a syscall too */
	idle_task = create_task(idle_func, NULL, 0);
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


/**
 * \brief Check if current process is the idle-process.
 * If so it should not be allowed to block in any way
 * @return 0 if the current process is not the idle-process
 */
static uint8_t is_background() {
	return (current == idle_task);
}

uint32_t time_slice_elapsed() {
	return uint32diff(last_context_time, read_timer32());
}

void sys_get_sysutime(uint32_t* time) {
	if (time == NULL)
		return;
	
	*time = read_timer32();
}

void sys_get_sysmtime(uint32_t* time) {
	if (time == NULL)
		return;
	
	*time = read_timer64() / 1000;
}

void sys_aos_hooks(struct aos_hooks* hooks) {
	_aos_hooks = hooks;	
}

void sys_usleep(uint32_t us) {
	struct list_head* e;
	struct list_head* insertion_point = NULL;
	uint32_t slice_elapsed = time_slice_elapsed();
	uint32_t time = us + slice_elapsed;

	if (time > MAX_TIME_SLICE_US) /** @fixme this work very poorly with dynamic timers-interrupt */
		time -= MAX_TIME_SLICE_US;

	if (is_background())
		return;
	
	/** @todo: Implement busywait here if delay is smaller than the minimum time-slice */
// 	if (us < MIN_TIME_SLICE_US) {
// 		uint32_t target_time = read_timer();
// 		sys_get_systime(&target_time);
// 		while (
// 	}

	/* Run through all sleeping processes all decrement the time our current
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
	if (is_background())
		return;
	//list_erase(&current->q); running proces is not in any queue
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


void sys_disable_irqs() {
	interrupts_disabled = 1;
}


void sys_enable_irqs() {
	interrupts_disabled = 0;
}
