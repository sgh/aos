#ifndef WAIT_H
#define WAIT_H

#include <mutex.h>
#include <list.h>

struct condition {
	mutex_t lock;
	struct list_head waiting;
};

void cond_init(struct condition* cond);
		
void cond_wait(struct condition* cond);

void cond_signal(struct condition* cond);

void cond_broadcast(struct condition* cond);


#endif