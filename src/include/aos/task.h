/*
	AOS - ARM Operating System
	Copyright (C) 2007  S�ren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#ifndef AOS_TASK_H
#define AOS_TASK_H

#include <aos/types.h>
#include <aos/list.h>
#include <aos/timer.h>
#include <aos/mutex.h>
#include <aos/semaphore.h>
#include <arch/cpu.h>

/**
 * \brief Define that eases creation of aos tasks.
 */
#define AOS_TASK __attribute__((noreturn))

enum task_state_enum  {RUNNING, READY, SLEEPING, BLOCKED, CRASHED};

/**
 * \brief A process
 */
struct task_t {
	/**
	 * \brief The name of the task.
	 */
	const char* name;

	/**
	 * \brief The priority of the process. Lower is better. This
	 * priority is dynamically adjusted.
	 */
	int8_t prio;

	/**
	 * \brief Node in process-queue.
	 */
	struct list_head q;	

	/**
   * \brief Node in global process-list
   */
	struct list_head glist;

	/**
	 * \brief Task state
	 */
	enum task_state_enum state;
	
	/**
	 * \brief  ticks to sleep.
	 */
	struct timer sleep_timer;

	/**
	 * \brief  ticks until timeout
	 */
	struct timer timeout_timer;

	/**
	 * \brief Return value for sleeps and blocks
	 */
	uint8_t sleep_result;

	/**
	 * \brief Pointer to the process's saved CPU-state (the registers, not the stack).
	 */
	struct context ctx;

	/**
	 * \brief Size of the space pointed to by ::fragment
	 * If the size is greater than zero, the process is using shared stacks.
	 */
	uint16_t stack_size;

	/**
	 * \brief The maximum registered stack usage
	 */
	uint16_t max_stack_size;

	/**
	 * \brief Ticks used by process
	 */
	uint32_t ticks;

	/**
	 * \brief Is the process-preemptive or not
	 */
	uint8_t preemptive;


	/**
	 * \brief Tick left in current timeslice
	 */
	uint32_t time_left;

	/**
	 * \brief Number of voluntary context-switches
	 */
	uint32_t voluntary_ctxt;

	/**
	 * \brief Number of nonvoluntary context-switches
	 */
	uint32_t nonvoluntary_ctxt;

	/**
	 * \brief Longest used timeslice
	 */
	uint32_t time_longest;

	/**
	 * \brief Fragment-store to store stack in when doing context-switch.
	 */
	struct fragment_store* fragment;

	int8_t resched;

	int32_t lock_count;

	mutex_t* wait_mutex;

	semaphore_t* wait_semaphore;

};

#define get_struct_task(Q) (container_of(Q,struct task_t,q))

#endif // _TASK_H_
