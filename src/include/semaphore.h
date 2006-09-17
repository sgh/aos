#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <types.h>
#include <list.h>

struct semaphore_t {
	int16_t counter;
	struct list_head waiting;
};

void sem_P(struct semaphore_t* s);
void sem_V(struct semaphore_t* s);

#endif
