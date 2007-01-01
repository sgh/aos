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
 * \brief Define that eases creation of aos tasks.
 */
#define AOS_TASK __attribute__((noreturn))

/**
 * \brief Minimum and maximum time-slice in microseconds.
 */
#define MAX_TIME_SLICE_US 1000
#define MIN_TIME_SLICE_US 100

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
	 * This number is lowered if processes steps in front of it in the readyQ
	 */
	int8_t prio;

	/**
	 * \brief The initial priority of the process. The priority of the process is
	 * reset to this value when inserted in the readyQ.
	 */
	uint8_t prio_initial;
	
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
