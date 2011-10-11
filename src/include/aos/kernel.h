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
#ifndef AOS_KERNEL_H
#define AOS_KERNEL_H

#include <aos/aos_module.h>

#include <aos/syscalls.h>
#include <aos/types.h>
#include <aos/task.h>
#include <aos/list.h>
#include <aos/runtime_check.h>

#include <aos/aos.h> /** @todo make this go away */



void context_init(struct context* ctx, void* kstack);
void context_set(struct context* ctx, uint8_t type, uint32_t val);
void context_destroy(struct context* ctx);
#define USER_STACK 0
#define USER_ENTRY 1
#define USER_ARG   2

/**
 * \brief Lock the schedueler
 */
void sched_lock(void);

/**
 * \brief Unlock and do context-switch if nescessary
 */
void sched_unlock(void);

/**
 * \brief Total number of context-switches in current uptime
 */
extern uint32_t num_context_switch;

/**
 * \brief linker-provided placement of stacks
 */
extern void __stack_usr_top__;
extern void __stack_usr_bottom__;
extern void __stack_svc_bottom__;
extern void __stack_irq_bottom__;

/**
 * \brief The idle task
 */
extern struct task_t idle_task;

/**
 * \brief The default preemptiveness of the system
 */
extern uint8_t default_preemptive;

/**
 * \brief The aos hooks
 */
extern struct aos_hooks* _aos_hooks;

/**
 * @todo write this
 */
extern struct aos_status _aos_status;


/**
 * \brief This queue of processes that are currently sleeping.
 */
extern struct list_head usleepQ;

/**
 * \brief This list contains all processes
 */
extern struct list_head process_list;

void init_task(struct task_t* task,taskFuncPtr entrypoint, void* arg, int8_t priority);
void destroy_task(struct task_t* t);


/**
 * \brief Wakeup the process and insert it in the readyQ.
 * @param task The task to wakeup.
 */
void process_wakeup(struct task_t* task);

void sched_clock(void);

void sched_sleep(uint16_t ms);

/**
 * \brief Check if current process is the idle-process.
 * If so it should not be allowed to block in any way
 * @return 0 if the current process is not the idle-process
 */
static inline uint8_t is_background(void) {
	return (current == &idle_task);
}


#endif // _KERNEL_H_
