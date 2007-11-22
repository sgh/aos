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

#include <syscalls.h>
#include <string.h>
#include <mm.h>

#include <kernel.h>
#include <task.h>
#include <types.h>
#include <fragment.h>
#include <config.h>

/** @todo This function is architechture specific and should be moved away */
/**
 * \brief Initializes a struct task
 * @param task The struct task to initialize
 * @param entrypoint The function to thread
 * @param arg. A void* argument for the process. Enables multiple equal
 * processes with different data.
 * @param priority The priority. Less is more :)
 */

void init_task(struct task_t* task,funcPtr entrypoint, void* arg, int8_t priority) {

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
	task->sleep_timer.type = TMR_STOP;
	task->timeout_timer.type = TMR_STOP;
	task->state = READY;
}

