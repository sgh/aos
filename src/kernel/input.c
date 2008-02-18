#include <stdio.h>
#include <stdint.h>

#define AOS_KERNEL_MODULE
#include <kernel.h>
#include <aos.h>
#include <mutex.h>
#include <input.h>

static uint32_t additional_keys[MAX_CONCURRENT_KEYS];
static uint32_t additional_times[MAX_CONCURRENT_KEYS];
static uint32_t active_key;
static uint32_t active_time;
static uint8_t pressed_idx;

static DECLARE_MUTEX_LOCKED(scancode_ready);
static uint32_t _getchar;
static DECLARE_MUTEX_UNLOCKED(_getchar_ready);
static unsigned int repeatcount = 0;


static void pressedlist_add(uint32_t time, uint32_t scancode) {
	int i;
// Move all keypresses a step right
	for (i=MAX_CONCURRENT_KEYS-1; i>0; i--) {
		additional_keys[i]  = additional_keys[i-1];
		additional_times[i] = additional_times[i-1];
	}

	additional_keys[0]  = active_key  = scancode;
	additional_times[0] = active_time = time;
}

static void pressedlist_remove(uint32_t scancode) {
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


static void scancode_press(uint32_t time, uint32_t scancode) {

// 	printf("press(%6u): %-4d ", time, scancode);

	pressedlist_add(time, scancode);
}


static void scancode_release(uint32_t scancode) {
// 	printf("release: %-4d ", scancode);

	pressedlist_remove(scancode);
}

void beep(void);
void beep_error(void);

void check_watches(uint32_t now) {
// 	if (additional_keys[0] == 17 && additional_keys[1] == 18 && (now - additional_times[0] > 2000) && (now - additional_times[1] > 2000))
// 		beep_error();
}


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

	// Run through released keys
	for (scancode = 1; release ; release >>= 1) {
		if (release & 1)
			scancode_release(scancode);
		scancode++;
	}

// 	if (press) {
	sys_get_sysmtime(&now);
// 		beep();
// 	}
	
	check_watches(now);

	for (scancode = 1; press ; press >>= 1) {
		if (press & 1)
			scancode_press(now, scancode);
		scancode++;
	}
	
	if (scancode_change)
		sys_mutex_unlock(&scancode_ready);
}

void aos_key_management_task(void* arg) {
	uint8_t last_scancode = 0;
	int s = 0;
	unsigned int timedwait = 0;
	
	while (1) {
		timedwait = 0;
		
		if (active_key != 0) {
			if (repeatcount == 0)
					timedwait = 500;
				else
					timedwait = 100;
		}

		if (mutex_timeout_lock(&scancode_ready, timedwait) == ETIMEOUT) {
			repeatcount++; // Key is repeated
		} else {
			if (last_scancode != active_key) { // New active key
				beep();
				repeatcount = 0;
			}
		}

		_getchar = active_key;
		mutex_unlock(&_getchar_ready);

		last_scancode = active_key;
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

