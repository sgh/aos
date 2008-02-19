#include <stdio.h>
#include <stdint.h>
#include <string.h>
#define AOS_KERNEL_MODULE
#include <kernel.h>
#include <aos.h>
#include <mutex.h>
#include <input.h>

#define BUFFER_SIZE 10
#define TOTAL_CONCURRENT_KEYS 6

static struct extended_char scancode_buffer[BUFFER_SIZE];
static unsigned int buffer_put_idx;
static unsigned int buffer_get_idx;

static uint32_t current_keys[TOTAL_CONCURRENT_KEYS];
static uint32_t current_times[TOTAL_CONCURRENT_KEYS];

static semaphore_t scancode_ready_sem;
static uint32_t _getchar;

static semaphore_t getchar_ready_sem;
static unsigned int repeatcount = 0;

const static struct input_hooks* inputhooks;

static void pressedlist_add(uint32_t time, uint32_t scancode) {
	int i;
// Move all keypresses a step right
	for (i=TOTAL_CONCURRENT_KEYS-1; i>0; i--) {
		current_keys[i]  = current_keys[i-1];
		current_times[i] = current_times[i-1];
	}

	current_keys[0]  = scancode;
	current_times[0] = time;
}


static void pressedlist_remove(uint32_t scancode) {
	int i;
	for (i=0; i<TOTAL_CONCURRENT_KEYS; i++) {
		if (current_keys[i] == scancode) {
			current_keys[i] = 0;
			current_times[i] = 0;
		}
	}
	
	// Fill 0 entries
	for (i=0; i<TOTAL_CONCURRENT_KEYS-1; i++) {
		if (current_keys[i] == 0) {
			current_keys[i] = current_keys[i+1];
			current_times[i] = current_times[i+1];
			current_keys[i+1] = 0;
			current_times[i+1] = 0;
		}
	}

}



/** @todo used user-space version of this to add keypresses from external switches etc. */
void aos_register_keyscan(uint32_t keyscan) {
	static uint32_t last_keyscan;
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
			pressedlist_remove(scancode);
		scancode++;
	}

	sys_get_sysmtime(&now);

	for (scancode = 1; press ; press >>= 1) {
		if (press & 1)
			pressedlist_add(now, scancode);
		scancode++;
	}
	
	if (scancode_change)
		sys_sem_up(&scancode_ready_sem);
}


void aos_key_management_task(UNUSED void* arg) {
	uint8_t last_scancode = 0;
	unsigned int timedwait = 0;
	
	while (1) {
		timedwait = 0;
		
		if (last_scancode != 0) {
			if (repeatcount == 0)
					timedwait = 500;
				else
					timedwait = 50;
		}

		if (sem_timeout_down(&scancode_ready_sem, timedwait) == ETIMEOUT) {
			repeatcount++; // Key is repeated
		} else {
			if (last_scancode != current_keys[0] && current_keys[0] != 0) { // New active key
				if (inputhooks && inputhooks->beep) inputhooks->beep();
				repeatcount = 0;
			}
		}

		_getchar = current_keys[0];
		if (inputhooks && inputhooks->keyfilter)
			_getchar = inputhooks->keyfilter(current_keys[0]);

		sem_up(&getchar_ready_sem);

		last_scancode = current_keys[0];
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
		if (timeout >= 0 && sem_timeout_down(&getchar_ready_sem, timeout) == ESUCCESS)
			fetchkey = 1;
			
		if (timeout < 0 && mutex_trylock(&getchar_ready_sem))
			fetchkey = 1;

		if (fetchkey) {
			for (i=0; i<MAX_CONCURRENT_KEYS; i++) {
				retchar.keys[i] = current_keys[i];
				retchar.times[i] = current_times[i];
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

void aos_input_init(const struct input_hooks* hooks) {
	inputhooks = hooks;
}

static void input_init(void) {
	sem_init(&getchar_ready_sem, 0);
	sem_init(&scancode_ready_sem, 0);
}

AOS_MODULE_INIT(input_init);
