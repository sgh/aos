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
#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <aos_module.h>

#include <syscalls.h>
#include <types.h>
#include <task.h>
#include <list.h>

#include <aos.h> /** @todo make this go away */


/**
 * \brief linker-provided placement of the stack
 * @todo it could be nice to have this detected automatically.
 */
extern uint32_t Top_Stack;

/**
 * \brief Internal function to get the usermode stackpointer.
 * @return The stackpointer from usermode
 */
__inline__ uint32_t get_usermode_sp(void);


/**
 * \brief Internal function used to get the current stack-pointer.
 * @return The current stackpointer
 */
__inline__ uint32_t get_sp(void);

/**
 * \brief The idle task
 */
extern struct task_t* idle_task;

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
 * \brief This queue hold processes that sleep.
 */
extern struct list_head usleepQ;


/**
 * \brief Wakeup the process and insert it in the readyQ
 * @param task The task to wakeup.
 */
void process_wakeup(struct task_t* task);

/**
 * \brief Unmask all interrupts
 */
void enable_irqs(void);

/**
 * \brief Mask all interrupts
 */
void disable_irqs(void);


#endif // _KERNEL_H_
