#include <aos/aos.h>

#include <time.h>
#include <sys/time.h>
#include <unistd.h>

void yield(void)
{
	sched_yield();
}


void msleep(uint16_t ms)
{
	usleep(1000*ms);
}

void get_sysmtime(uint32_t* time)
{
	static time_t sec_offset=0;

	if(!time) return;

	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC_RAW, &now);

	time_t sec = now.tv_sec;
	long msec = now.tv_nsec/1000000;

	if ( sec_offset == 0 )
		sec_offset = sec;

	sec -= sec_offset;

	*time = 1000*sec + msec;
}

// overflow after ~70min when using this function
void get_sysutime(uint32_t* time)	// not used
{
	get_sysmtime(time);
	*time *= 1000;
}

void aos_init(void)
{
}
