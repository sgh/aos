#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <types.h>
#include <mutex.h>
#include <list.h>

struct semaphore_t {
	int16_t count;
	mutex_t lock;
	struct list_head waiting;
};

void sem_init(struct semaphore_t* s, uint16_t count);
void sem_down(struct semaphore_t* s);
void sem_up(struct semaphore_t* s);

#endif
