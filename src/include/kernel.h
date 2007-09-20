/*
	AOS - ARM Operating System
	Copyright (C) 2007  S�ren Holm (sgh@sgh.dk)

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
#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <aos_module.h>

#include <syscalls.h>
#include <types.h>
#include <task.h>
#include <list.h>
#include <runtime_check.h>

#include <aos.h> /** @todo make this go away */

/**
 * \brief Total number of context-switches in current uptime
 */
extern uint32_t num_context_switch;

/**
 * \brief linker-provided placement of stacks
 */
extern uint32_t __stack_usr_top__;
extern uint32_t __stack_usr_bottom__;
extern uint32_t __stack_svc_bottom__;
extern uint32_t __stack_irq_bottom__;

/**
 * \brief The idle task
 */
extern struct task_t idle_task;

/**
 * \brief The aos hooks
 */
extern struct aos_hooks* _aos_hooks;

/**
 * @todo write this
 */
extern struct aos_status _aos_status;

/**
 * \brief Shall we do proccess-shift.
 */
extern uint8_t do_context_switch;


/**
 * \brief This queue of processes that are currently sleeping.
 */
extern struct list_head usleepQ;

/**
 * \brief This list contains all processes
 */
extern struct list_head process_list;

void init_task(struct task_t* task,funcPtr entrypoint, void* arg, int8_t priority);


/**
 * \brief Wakeup the process and insert it in the readyQ.
 * @param task The task to wakeup.
 */
void process_wakeup(struct task_t* task);

void sched_clock(void);

/**
 * \brief Check if current process is the background-process.
 * @return 1 if the current process is the background-process
 */
uint8_t is_background(void);

void sched_unlock(void);
void sched_lock(void);


#endif // _KERNEL_H_
