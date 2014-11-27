/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#define AOS_KERNEL_MODULE

#include <aos/syscalls.h>
#include <string.h>
#include <aos/mm.h>

#include <aos/kernel.h>
#include <aos/task.h>
#include <aos/types.h>
#include <aos/fragment.h>
#include <aos/config.h>

/** @todo This function is architechture specific and should be moved away */
/**
 * \brief Initializes a struct task
 * @param task The struct task to initialize
 * @param entrypoint The function to thread
 * @param arg. A void* argument for the process. Enables multiple equal
 * processes with different data.
 * @param priority The priority. Less is more :)
 */

void init_task(struct task_t* task, taskFuncPtr entrypoint, void* arg, int8_t priority) {

// 	char* ustack = sys_malloc(USIZE);
	void* ustack = (char*)&__stack_usr_top__;
	void* kstack = sys_malloc(KSIZE);

// 	ustack += USIZE;
	kstack += KSIZE;

	memset(task, 0, sizeof(struct task_t));

	task->fragment = NULL;
// 	task->stack = NULL;

	context_init(&task->ctx, kstack);
	context_set(&task->ctx, USER_STACK, (uint32_t)ustack);
	context_set(&task->ctx, USER_ENTRY, (uint32_t)entrypoint);
	context_set(&task->ctx, USER_ARG, (uint32_t)arg);

	task->prio = priority;
	task->time_left = QUANTUM;
	task->sleep_timer.type = TMR_STOP;
	task->timeout_timer.type = TMR_STOP;
	task->preemptive = default_preemptive;
	task->state = READY;
}

void destroy_task(struct task_t* t) {
	context_destroy(&t->ctx);
}
