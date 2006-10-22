#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <types.h>
#include <list.h>

struct semaphore_t {
	int16_t counter;
	struct list_head waiting;
};

void sem_down(struct semaphore_t* s);
void sem_up(struct semaphore_t* s);

#endif
