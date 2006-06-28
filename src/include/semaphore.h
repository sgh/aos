#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <types.h>
#include <queue.h>

struct semaphore_t {
	int16 counter;
	struct list_head waiting;
};



#endif
