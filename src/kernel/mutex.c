
#include <spinlock.h>
#include <mutex.h>
#include <task.h>
#include <syscalls.h>

void mutex_lock(mutex_t* m) {
	spinlock_lock(&m->spinlock);

	if (!m->lock) { // Mutex is not locked - lock it
		m->lock = 1;
		spinlock_unlock(&m->spinlock);
		return;
	}

	spinlock_unlock(&m->spinlock);
	
	block(&m->waiting);
}


void mutex_unlock(mutex_t* m) {
	struct task_t* next;
	
	spinlock_lock(&m->spinlock);
	if (list_isempty(&m->waiting)) { // If noone is waiting
		m->lock = 0;
		spinlock_unlock(&m->spinlock);
		return;
	}

	next = get_struct_task(list_get_front(&m->waiting));
	list_erase(/*&m->waiting,*/ &next->q);
	spinlock_unlock(&m->spinlock);
	
	unblock(next);
}

