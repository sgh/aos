#ifndef MUTEX_H
#define MUTEX_H

#include <list.h>

typedef struct {
	uint32_t spinlock;
	uint8_t lock;
	struct list_head waiting;
} mutex_t;

void mutex_lock(mutex_t* m);
void mutex_unlock(mutex_t* m);
void mutex_init(mutex_t* m);

#endif
