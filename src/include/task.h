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
#ifndef _TASK_H_
#define _TASK_H_

#include <types.h>
#include <list.h>
#include <timer.h>

/**
 * \brief This is defined if the system should be using a shared stack.
 * create_task does not currently support creating tasks with seperate stacks.
 */
#define SHARED_STACK

/**
 * \brief Define that eases creation of aos tasks.
 */
#define AOS_TASK __attribute__((noreturn))

/**
 * \brief Minimum and maximum time-slice in microseconds.
 */
#define MAX_TIME_SLICE_US 1000
#define MIN_TIME_SLICE_US 1000

/**
 * \brief A process
 */
struct task_t {
	/**
	 * \brief The name of the task.
	 */
	char* name;

	/**
	 * \brief The priority of the process. Lower is better. This
	 * priority is dynamically adjusted.
	 */
	int8_t prio;

	/**
	 * \brief Node in process-queue.
	 */
	struct list_head q;	
	
	enum task_state {RUNNING, READY, BLOCKED, CRASHED} state;
	
	/**
	 * \brief  useconds to sleep.
	 */
	struct timer sleep_timer;

	/**
	 * \brief Average sleep-time. Used for dynamic prioritize
	 */
	//uint32_t avr_sleep_time;

	/**
	 * \brief Pointer to the process's saved CPU-state (the registers, not the stack).
	 */
	REGISTER_TYPE *context;

	/**
	 * \brief Size of the space pointed to by ::fragment
	 * If the size is greater than zero, the process is using shared stacks.
	 */
	uint16_t stack_size;

	/**
	 * \brief Fragment-store to store stack in when doing context-switch.
	 */
	struct fragment_store* fragment;

};

#define get_struct_task(Q) (container_of(Q,struct task_t,q))

#endif // _TASK_H_
