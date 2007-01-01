#include <aos.h>

void spinlock_lock(spinlock_t* lock) {
	while (atomic_xchg(lock,1))
		yield();
}


void spinlock_unlock(spinlock_t* lock) {
	*lock = 0;
}
