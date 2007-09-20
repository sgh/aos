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

void syscall_ret(void);

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
	struct kern_regs* k;
	struct cpu_regs* u;

	//REGISTER_TYPE cpsr = 0x00000010; // User-mode
	REGISTER_TYPE cpsr = 0x0000001F; // System-mode
	char* ustack = sys_malloc(USIZE);
	char* kstack = sys_malloc(KSIZE);
	ustack += USIZE;
	kstack += KSIZE;

	// If address is thumb we must set it in the PSR
	if (((uint32_t)entrypoint & 1) == 1) 
		cpsr |= 0x20;	// Set thumb bit

	memset(task, 0, sizeof(struct task_t));

	task->fragment = NULL;

	task->ctx.uregs = (struct cpu_regs*)(kstack - sizeof(struct cpu_regs));

	k = &task->ctx.kregs;

	k->lr = (uint32_t)syscall_ret;
	k->sp = (uint32_t)task->ctx.uregs;

	u = task->ctx.uregs;
	u->r0 = (uint32_t)arg;
	u->r1 = 0x11111111;
	u->r2 = 0x22222222;
	u->r3 = 0x33333333;
	u->svc_sp = (uint32_t)kstack;
	u->sp = (uint32_t)ustack;
	u->pc = u->lr = (uint32_t)(entrypoint); // Entrypoint
	u->cpsr = cpsr;

	task->prio = priority;
	task->sleep_timer.type = TMR_STOP;
	task->state = READY;
}

