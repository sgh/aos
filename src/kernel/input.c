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
#define AOS_KERNEL_MODULE
#include <kernel.h>
#include <aos.h>
#include <mutex.h>
#endif
#include <input.h>


#ifdef __arm__
#define printf(args,...)
#endif


#define bin(a,b,c,d,e,f,g,h) ((a)*128+(b)*64+(c)*32+(d)*16+(e)*8+(f)*4+(g)*2+(h)*1)

#ifndef __arm__
static uint32_t mtime_bias;


void get_sysmtime(uint32_t* time) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	*time = (tv.tv_sec*1000000 + tv.tv_usec)/1000 - mtime_bias;
}

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
#endif

static uint32_t additional_keys[MAX_CONCURRENT_KEYS];
static uint32_t additional_times[MAX_CONCURRENT_KEYS];
static uint32_t active_key;
static uint32_t active_time;
static uint8_t pressed_idx;

static DECLARE_MUTEX_LOCKED(scancode_ready);
static uint32_t _getchar;
static DECLARE_MUTEX_UNLOCKED(_getchar_ready);



void pressedlist_add(uint32_t time, uint32_t scancode) {
	int i;
// Move all keypresses a step right
	for (i=MAX_CONCURRENT_KEYS-1; i>0; i--) {
		additional_keys[i]  = additional_keys[i-1];
		additional_times[i] = additional_times[i-1];
	}

	additional_keys[0]  = active_key  = scancode;
	additional_times[0] = active_time = time;
	
// 	for (i=0; i<MAX_CONCURRENT_KEYS; i++)
// 		additional_times[0] = 3;
}

void pressedlist_remove(uint32_t scancode) {
	uint8_t found = 0;
	int i;
	for (i=0; i<MAX_CONCURRENT_KEYS; i++) {
		if (additional_keys[i] == scancode) {
			additional_keys[i] = 0;
			additional_times[i] = 0;
		}
	}
	
	// Fill 0 entries
	for (i=0; i<MAX_CONCURRENT_KEYS-1; i++) {
		if (additional_keys[i] == 0) {
			additional_keys[i] = additional_keys[i+1];
			additional_times[i] = additional_times[i+1];
			additional_keys[i+1] = 0;
			additional_times[i+1] = 0;
		}
	}

	if (active_key == scancode) {
		active_key = 0;
		active_time = 0;
	}
}


void scancode_press(uint32_t time, uint32_t scancode) {

	printf("press(%6u): %-4d ", time, scancode);

	pressedlist_add(time, scancode);
}

void scancode_release(uint32_t scancode) {
	printf("release: %-4d ", scancode);

	pressedlist_remove(scancode);
}

void beep(void);

void aos_register_keyscan(uint32_t keyscan) {
	static uint32_t last_keyscan;
	int i;
	uint32_t now = 0;
	uint32_t scancode;
	uint32_t press = keyscan & ~last_keyscan;
	uint32_t release = last_keyscan & ~keyscan;
	uint8_t scancode_change = 0;
	last_keyscan = keyscan;

	if ((press != 0) || (release |= 0))
		scancode_change = 1;
	
	printf("keyscan:0x%02X ", keyscan);
	
	// Run through released keys
	for (scancode = 1; release ; release >>= 1) {
		if (release & 1)
			scancode_release(scancode);
		scancode++;
	}
	
	if (press) {
		sys_get_sysmtime(&now);
// 		beep();
	}
	for (scancode = 1; press ; press >>= 1) {
		if (press & 1)
			scancode_press(now, scancode);
		scancode++;
	}
	
	if (scancode_change) {
		printf(" %d[", active_key);
		for (i=0; i<2; i++)
			printf("%d ", additional_keys[i]);
		printf("]\n");
		sys_mutex_unlock(&scancode_ready);
	} else {
		printf("\n");
	}
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

static unsigned int repeatcount = 0;

void aos_key_management_task(void* arg) {
	uint8_t last_scancode = 0;
	int s = 0;
	unsigned int timedwait = 0;
	
	while (1) {

		if (active_key != 0) {

			if (repeatcount == 0)
				timedwait = 500;
			else
				timedwait = 100;


			if (mutex_timeout_lock(&scancode_ready, timedwait) == ETIMEOUT)
				s = -1;
			else
				s=0;
		}	else {
			timedwait = 0;
			mutex_lock(&scancode_ready);
		}
		
		if (s == -1)  {
			repeatcount++;
			printf("repeat %d", active_key);
		} else {
			if (active_key != 0) {
				printf("beep ");
				beep();
			}
			printf("change %d", active_key);
			repeatcount = 0;
		}
		
		_getchar = active_key;
		mutex_unlock(&_getchar_ready);

		last_scancode = active_key;
		printf("\n");
	}
}


struct extended_char aos_extended_getchar(int timeout) {
	struct extended_char retchar;
	uint8_t fetchkey;
	int i;

	do {
		fetchkey = 0;
		if (timeout >= 0 && mutex_timeout_lock(&_getchar_ready, timeout) == ESUCCESS)
			fetchkey = 1;
			
		if (timeout < 0 && mutex_trylock(&_getchar_ready))
			fetchkey = 1;

		for (i=0; i<MAX_CONCURRENT_KEYS; i++) {
			retchar.keys[i] = additional_keys[i];
			retchar.times[i] = additional_times[i];
		}
		retchar.repeatcount = repeatcount;
		
		if (timeout != 0) // When we get a timeout we must not loop
			break;
	} while (retchar.keys[0] == 0);

	return retchar;
}

uint32_t aos_getchar(int timeout) {
	struct extended_char ch;

	ch = aos_extended_getchar(timeout);
	return ch.keys[0];
}


#ifndef __arm__
void consumer_task(void* arg) {
	while (1) {
		printf("%s: %d\n", __FUNCTION__, aostk_getchar());
	}
}


int main(/*int argc, const char* argv[]*/) {
	unsigned int i;
	pthread_t keytask;

	
	sem_init(&scancode_ready, 0, 0);
	sem_init(&_getchar_ready, 0, 0);
	pthread_create(&keytask, NULL, aos_key_management_task, NULL);
	pthread_create(&keytask, NULL, consumer_task, NULL);
	
	get_sysmtime(&mtime_bias);
	for (i = 0; i<sizeof(keyscans); i++) {
		aos_register_keyscan(keyscans[i]);
		usleep(5000000);
	}
	
// 	printf("bx: %d\n", bit_count(64+5));
	return 0;
}
#endif
