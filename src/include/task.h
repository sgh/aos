#ifndef _TASK_H_
#define _TASK_H_

#include <types.h>
#include <list.h>

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
	uint32 sleep_time;
	
	funcPtr entrypoint;
	
	/**
	 * \brief Pointer to the process' saved stack. if the process is using the systems shared stack.
	 */
	REGISTER_TYPE *stack;
	
	/**
	 * \brief Size of the space pointed to by *stack
	 * If the size is greater than zero, the process is using shared stacks.
	 */
	uint16 stack_size;
	
	/**
	 * \brief  This will hold the process' complete stack if this process is uses a seperate stack. ÆØÅæøå
	 * If running with shared stack this space only contains the saved processor-state.
	 */
	uint8 stacksave[500]; /**< @todo convert this to at pointer to the memory-space */
};

#define get_struct_task(Q) (container_of(Q,struct task_t,q))

#endif // _TASK_H_
