#ifndef _TASK_H_
#define _TASK_H_

#include <types.h>
#include <list.h>

/**
 * \brief This is defined if the system should be using a shared stack.
 * create_task does not currently support creating tasks with seperate stacks.
 */
#define SHARED_STACK

/**
 * \brief A process
 */
struct task_t {
	/**
	 * \brief The name of the task.
	 */
	char* name;

	/**
	 * \brief The priority of the process. Lower is higher.
	 */
	int8_t priority;
	
	/**
	 * \brief Node in process-queue.
	 */
	struct list_head q;	
	
	enum task_state {RUNNING, READY, BLOCKED, CRASHED} state;
	
	/**
	 * \brief  useconds to sleep.
	 */
	uint32_t sleep_time;

	/**
	 * \brief Pointer to the process's saved CPU-state.
	 */
	REGISTER_TYPE *context;

	/**
	 * \brief Size of the space pointed to by *stack
	 * If the size is greater than zero, the process is using shared stacks.
	 */
	uint16_t stack_size;

	/**
	 * \brief Fragment-store to store stack in when doint context-switch.
	 */
	struct fragment_store* fragment;
};

#define get_struct_task(Q) (container_of(Q,struct task_t,q))

#endif // _TASK_H_
