#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <types.h>
#include <list.h>

struct semaphore_t {
	int16 counter;
	struct list_head waiting;
};



#endif
