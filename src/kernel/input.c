#include <stdio.h>
#include <stdint.h>

#ifndef __arm__
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#else
#include <aos.h>
#include <mutex.h>
#endif


#ifdef __arm__
#define printf(args,...)
#endif


#define bin(a,b,c,d,e,f,g,h) ((a)*128+(b)*64+(c)*32+(d)*16+(e)*8+(f)*4+(g)*2+(h)*1)

static uint32_t mtime_bias;

#ifndef __arm__
void get_sysmtime(uint32_t* time) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	*time = (tv.tv_sec*1000000 + tv.tv_usec)/1000 - mtime_bias;
}
#endif

uint8_t keyscans[] = {
	bin(0,0,0,0,0,0,0,0),
	bin(1,0,0,0,0,0,0,0),
	bin(1,0,0,0,0,0,0,0),
	bin(1,0,0,0,0,0,0,1),
	bin(1,0,0,0,0,0,0,1),
	bin(1,0,0,0,0,0,0,1),
	bin(1,0,0,0,0,0,0,0),
	bin(1,0,0,0,0,0,0,0),
	bin(1,0,0,0,0,0,0,1),
	bin(1,0,0,0,0,0,0,1),
	bin(1,0,0,0,0,0,1,1),
	bin(1,0,0,0,0,0,1,1),
	bin(1,0,0,0,0,1,1,1),
	bin(1,0,0,0,0,1,1,1),
	bin(1,0,0,0,0,0,0,0),
	bin(1,0,0,0,0,0,0,0),
	bin(0,0,0,0,0,0,0,0),
	bin(0,0,0,0,0,0,0,0),
	bin(0,0,0,0,0,0,0,0),
};

static uint8_t additional_keys[2];
static uint8_t active_scancode;
static uint8_t active_time;
static uint8_t pressed_idx;

#ifndef __arm__
static sem_t scancode_ready;
#else
	DECLARE_MUTEX_LOCKED(scancode_ready);
#endif

static uint32_t _getchar;

#ifndef __arm__
static sem_t _getchar_ready;
#else
	DECLARE_MUTEX_UNLOCKED(_getchar_ready);
#endif


void pressedlist_add(uint32_t time, uint32_t scancode) {
	pressed_idx++;
	if (pressed_idx > 1)
		pressed_idx = 0;

	additional_keys[pressed_idx] = active_scancode = scancode;
	active_time = time;
}

void pressedlist_remove(uint32_t scancode) {
	int i;
	for (i=0; i<2; i++) {
		if (additional_keys[i] == scancode)
			additional_keys[i] = 0;
	}
	
	if (active_scancode == scancode)
		active_scancode = 0;
}

void scancode_press(uint32_t time, uint32_t scancode) {

	printf("press(%6u): %-4d ", time, scancode);

	pressedlist_add(time, scancode);
}

void scancode_release(uint32_t scancode) {
	printf("release: %-4d ", scancode);

	pressedlist_remove(scancode);
}


void register_keyscan(uint8_t keyscan) {
	static uint8_t last_keyscan;
	int i;
	uint32_t now = 0;
	uint32_t scancode = 1;
	uint8_t press = keyscan & ~last_keyscan;
	uint8_t release = last_keyscan & ~keyscan;
	uint8_t scancode_change = (press != 0) | (release |= 0);
	last_keyscan = keyscan;
	
	printf("keyscan:0x%02X ", keyscan);
	
	// Run through released keys
// 	scancode = 1;
	for (scancode = 1; release ; release >>= 1) {
		if (release & 1)
			scancode_release(scancode);
		scancode++;
	}
	
	if (press)
		get_sysmtime(&now);
	for (scancode = 1; press ; press >>= 1) {
		if (press & 1)
			scancode_press(now, scancode);
		scancode++;
	}
	
	if (scancode_change) {
		printf(" %d[", active_scancode);
		for (i=0; i<2; i++)
			printf("%d ", additional_keys[i]);
		printf("]\n");
#ifndef __arm__
		sem_post(&scancode_ready);
#else
		mutex_unlock(&scancode_ready);
#endif
	} else
		printf("\n");
}


// unsigned bit_count(unsigned n)
//  {
//  unsigned count;
//  
//  for (count = 0; n; n &= n - 1) {
// 	 printf("n: %d\n",n);
//  	++count;
//  }
//  return count;
// }


void key_management_task(void* arg) {
#ifndef __arm__
	struct timespec ts;
#endif
	uint8_t last_scancode = 0;
	int s = 0;
	int timedwait = 0;
	unsigned int repeatcount = 0;
	
	while (1) {
		
		if (active_scancode != 0) {
			
#ifndef __arm__
			clock_gettime(CLOCK_REALTIME, &ts);
#endif
			if (repeatcount == 0) {
#ifndef __arm__
				ts.tv_sec += 1;
#else
				timedwait = 1000;
#endif
			} else {
#ifndef __arm__
// 				ts.tv_sec += 0;
				ts.tv_nsec += 500000000;
				while (ts.tv_nsec > 999999999) {
					ts.tv_nsec = ts.tv_nsec - 999999999;
					ts.tv_sec += 1;
				}
#else
				timedwait = 500;
#endif
			}

#ifndef __arm__
			s = sem_timedwait(&scancode_ready, &ts);
#else
			s = mutex_timeout_lock(&scancode_ready, timedwait);
#endif
		}	else {
			timedwait = 0;
#ifndef __arm__
			sem_wait(&scancode_ready);
#else
			mutex_lock(&scancode_ready);
#endif
		}
		
		printf("SCANCODE HANDLER ");
		
		if (s == -1)  {
			repeatcount++;
			printf("repeat %d", active_scancode);
		} else {
			if (active_scancode != 0)
				printf("beep ");
			printf("change %d", active_scancode);
			repeatcount = 0;
		}
		
		_getchar = active_scancode;
#ifndef __arm__
		sem_post(&_getchar_ready);
#else
		mutex_unlock(&_getchar_ready);
#endif

		last_scancode = active_scancode;
		printf("\n");
	}
}


uint32_t aostk_getchar(void) {
#ifndef __arm__
	sem_wait(&_getchar_ready);
#else
	mutex_lock(&_getchar_ready);
#endif
	return _getchar;
}


void consumer_task(void* arg) {
	while (1) {
		printf("%s: %d\n", __FUNCTION__, aostk_getchar());
	}
}


#ifndef __arm__
int main(/*int argc, const char* argv[]*/) {
	unsigned int i;
	pthread_t keytask;

	
	sem_init(&scancode_ready, 0, 0);
	sem_init(&_getchar_ready, 0, 0);
	pthread_create(&keytask, NULL, key_management_task, NULL);
	pthread_create(&keytask, NULL, consumer_task, NULL);
	
	get_sysmtime(&mtime_bias);
	for (i = 0; i<sizeof(keyscans); i++) {
		register_keyscan(keyscans[i]);
		usleep(5000000);
	}
	
// 	printf("bx: %d\n", bit_count(64+5));
	return 0;
}
#endif
