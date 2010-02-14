#include <aos/aos.h>

#include <time.h>
#include <sys/time.h>

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
	static uint32_t offset=0;

	if(!time) return;

	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);

	int sec = now.tv_sec/* - ref.tv_sec*/;
	int msec = (now.tv_nsec/* - ref.tv_nsec*/)/1000000;
// 	if(msec < 0) {
// 		sec -= 1;
// 		msec += 1000;
// 	}

	if (!offset)
		offset = sec;

	sec -= offset;

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
// 	clock_gettime(CLOCK_REALTIME, &ref);
}
