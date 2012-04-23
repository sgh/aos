#include <aos/mutex.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

void mutex_lock(mutex_t *m)
{
	pthread_mutex_lock(&m->lock);
	while(m->cnt == 0)
		pthread_cond_wait(&m->cond, &m->lock);
	m->cnt = 0;
//	pthread_cond_signal(&m->cond);
	pthread_mutex_unlock(&m->lock);
}


void mutex_unlock(mutex_t *m)
{
	pthread_mutex_lock(&m->lock);
	m->cnt = 1;
	pthread_cond_signal(&m->cond);
	pthread_mutex_unlock(&m->lock);
}


uint8_t mutex_timeout_lock(mutex_t* m, uint32_t timeoutms)
{
	if ( timeoutms == 0 )
	{
		mutex_lock(m);
		return ESUCCESS;
	}

	struct timespec wakeup;
	clock_gettime( CLOCK_REALTIME, &wakeup );
	wakeup.tv_sec += timeoutms/1000;
	wakeup.tv_nsec += 1000000*(timeoutms%1000);

	long overrun = wakeup.tv_nsec/1000000000;
	wakeup.tv_nsec %= 1000000000;
	wakeup.tv_sec += (time_t) overrun;

	pthread_mutex_lock(&m->lock);

	int retval = 0;
	while(retval == 0 && m->cnt == 0)
		retval = pthread_cond_timedwait(&m->cond, &m->lock, &wakeup);

	if(retval == 0) { // Not timed out
		m->cnt = 0;
//		pthread_cond_signal(&m->cond);
	}

	pthread_mutex_unlock(&m->lock);
	return (retval == 0 ? ESUCCESS : ETIMEOUT);
}


uint8_t mutex_trylock(mutex_t *m)
{
	uint8_t retval = 0;

	pthread_mutex_lock(&m->lock);
	if(m->cnt != 0) { // Unlocked - lock it
		m->cnt = 0;
//		pthread_cond_signal(&m->cond);
		retval = 1;
	}
	pthread_mutex_unlock(&m->lock);

	return retval;
}


void mutex_init(mutex_t *m)
{
	pthread_mutex_init(&m->lock, NULL);
	pthread_cond_init(&m->cond, NULL);
	m->cnt = 1;
}
