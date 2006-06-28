#ifndef _TASK_H_
#define _TASK_H_

#include <types.h>
#include <list.h>

struct task_t {
	char* name;								// The name of the task
	struct list_head q;		// Queue
	enum task_state {RUNNING, READY, BLOCKED} state;
	uint32 sleep_time;				// useconds to sleep
	funcPtr entrypoint;
	REGISTER_TYPE *stack;			// Pointer to the stack
	uint16 stack_size;				// Size of the space pointed to by *stack
	uint8 stacksave[500];
};

#define get_struct_task(Q) (container_of(Q,struct task_t,q))

#endif // _TASK_H_
