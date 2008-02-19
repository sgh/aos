#include <stdio.h>
#include <stdint.h>
#include <string.h>
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
	pressedlist_add(time, scancode);
}


static void scancode_release(uint32_t scancode) {
	pressedlist_remove(scancode);
}

void beep(void);
void beep_error(void);
uint_fast32_t quickmenu_keyhook(uint_fast32_t key);


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

	sys_get_sysmtime(&now);

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
					timedwait = 50;
		}

		if (mutex_timeout_lock(&scancode_ready, timedwait) == ETIMEOUT) {
			repeatcount++; // Key is repeated
		} else {
			if (last_scancode != active_key && active_key != 0) { // New active key
				beep();
				repeatcount = 0;
			}
		}

		_getchar = active_key;
		_getchar = quickmenu_keyhook(active_key);
		mutex_unlock(&_getchar_ready);

	 last_scancode = active_key;
	}
}



uint32_t aos_concurrent_keys(struct extended_char* exchar, uint32_t keys[MAX_CONCURRENT_KEYS]) {
	uint32_t maxtime = 0;
	uint32_t now = 0;
	int i,k;
	uint8_t found = 0;

	// Check to align character count
	for (i=0; i<MAX_CONCURRENT_KEYS; i++) {
		if (keys[i])
			found++;
		if (exchar->keys[i])
			found--;
	}

	// If count does not match the concurrent keypress is not valid
	if (found != 0)
		return 0;

	for (i=0; i<MAX_CONCURRENT_KEYS; i++) {

		uint32_t act_key = exchar->keys[i];

		if (!act_key)
			continue;

		found = 0;

		// Check for key existence
		for (k=0; k<MAX_CONCURRENT_KEYS; k++) {

			if (act_key == keys[k]) 
				found = 1;
		}

		if (found) {
			if (exchar->times[i] > maxtime)
				maxtime = exchar->times[i];
		} else
			break;
	}

	get_sysmtime(&now);

	if (found)
		return now - maxtime + 1; // At least one ms

	return 0;
}




struct extended_char aos_extended_getchar(int timeout) {
	struct extended_char retchar;
	uint8_t fetchkey;
	int i;

	memset(&retchar, 0, sizeof(retchar));
	do {
		fetchkey = 0;
		if (timeout >= 0 && mutex_timeout_lock(&_getchar_ready, timeout) == ESUCCESS)
			fetchkey = 1;
			
		if (timeout < 0 && mutex_trylock(&_getchar_ready))
			fetchkey = 1;

		if (fetchkey) {
			for (i=0; i<MAX_CONCURRENT_KEYS; i++) {
				retchar.keys[i] = additional_keys[i];
				retchar.times[i] = additional_times[i];
			}
			retchar.keys[0] = _getchar;
			retchar.repeatcount = repeatcount;
		}
		
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

