#ifndef _TASK_H_
#define _TASK_H_

#include <types.h>
#include <list.h>

/**
 * \brief A process
 */
struct task_t {
	/**
	 * \brief The name of the task.
	 */
	char* name;
	
	/**
	 * \brief Node in process-queue.
	 */
	struct list_head q;	
	
	enum task_state {RUNNING, READY, BLOCKED} state;
	
	/**
	 * \brief  useconds to sleep.
	 */
	uint32_t sleep_time;
	
	funcPtr entrypoint;
	
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
	 * \brief  This will hold the process's complete stack.
	 * The content does differ a bit depending of the stack-type
	 * - Seperate stack: The memor hold the process's running stack.
	 * - Shared stack: The process's stack is stored here a context-switch.
	 * 
	 * The latter is the most common setup for most applications.
	 */
	uint8_t stack[600]; /**< @todo convert this to a pointer to the memory-space */
	
	void* malloc_stack;
	struct fragment_store* fragment;
};

#define get_struct_task(Q) (container_of(Q,struct task_t,q))

#endif // _TASK_H_
