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
	//REGISTER_TYPE cpsr = 0x00000010; // User-mode
	REGISTER_TYPE cpsr = 0x0000001F; // System-mode

	// If address is thumb we must set it in the PSR
	if (((uint32_t)entrypoint & 1) == 1) 
		cpsr |= 0x20;	// Set thumb bit

	memset(task, 0, sizeof(struct task_t));
	task->context = sys_malloc(sizeof(struct cpu_context));
	task->fragment = NULL;
	task->context->pc = (uint32_t)(entrypoint);                                  // Entrypoint
#ifdef SHARED_STACK
	task->context->sp = (uint32_t)&__stack_usr_top__;  // Shared stack SP
#else
	#error Seperate stacks does not work
	task->context->sp = (uint32_t)task->context + (1024 * sizeof(REGISTER_TYPE));  // Seperate stack SP
#endif
	task->context->lr = 0x12345678;
	task->context->r0 = (uint32_t)arg;
	task->context->cpsr = cpsr;
	task->context->r1 = 0x1;
	task->context->r2 = 0x2;
	task->context->r3 = 0x3;
	task->context->r4 = 0x4;
	task->context->r5 = 0x5;
	task->context->r6 = 0x6;
	task->context->r7 = 0x7;
	task->context->r8 = 0x8;
	task->context->r9 = 0x9;
	task->context->r10 = 0x10;
	task->context->r11 = 0x11;
	task->context->r12 = 0x12;
	task->prio = priority;
	task->sleep_timer.type = TMR_STOP;
	task->state = READY;
}

