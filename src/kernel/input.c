#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define AOS_KERNEL_MODULE
#include <aos/kernel.h>
#include <aos/aos.h>
#include <aos/mutex.h>
#include <aos/input.h>
#include <aos/irq.h>

#define BUFFER_SIZE 5
#define TOTAL_CONCURRENT_KEYS 6

static struct extended_char char_buffer[BUFFER_SIZE];
static unsigned int buffer_put_idx;
static unsigned int buffer_get_idx;
static DECLARE_MUTEX_UNLOCKED(char_buffer_lock);

static uint32_t current_keys[TOTAL_CONCURRENT_KEYS];
static uint32_t current_times[TOTAL_CONCURRENT_KEYS];

static semaphore_t keyscan_ready_sem;

static semaphore_t getchar_ready_sem;
static unsigned int repeatcount = 0;

static const struct input_hooks* inputhooks;

static uint32_t current_keyscan;

static void generic_register_keyscan(uint32_t keyscan, uint8_t irq) {
	uint32_t now = 0;
	uint32_t scancode;

	if (current_keyscan == keyscan)
		return;

	if (!irq) irq_lock();

	current_keyscan = keyscan;
	
	if (irq)
		sys_sem_up(&keyscan_ready_sem);
	else
		sem_up(&keyscan_ready_sem);
	
	if (!irq) irq_unlock();
}


void aos_register_keyscan_irq(uint32_t keyscan) {
	generic_register_keyscan(keyscan, 1);
}

void aos_register_keyscan(uint32_t keyscan) {
	generic_register_keyscan(keyscan, 0);
}

void aos_register_keypress(uint32_t scancode) {
#warning implement me
// 	struct extended_char xchar;
// 	
// 	memset(&xchar, 0, sizeof(xchar));
// 	xchar.keys[0] = scancode;
// 	
// 	mutex_lock(&char_buffer_lock);
// 	
// 	buffer_put_idx++;
// 	if (buffer_put_idx == BUFFER_SIZE)
// 		buffer_put_idx = 0;
// 	
// 	char_buffer[buffer_put_idx] = xchar;
// 
// 	if (inputhooks && inputhooks->beep) inputhooks->beep();
// 	
// 	if (inputhooks && inputhooks->keyfilter)
// 		inputhooks->keyfilter(&char_buffer[buffer_put_idx]);
// 
// 	sem_up(&getchar_ready_sem);
// 	
// 	mutex_unlock(&char_buffer_lock);
}

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

	// Fill 0 entries (except the first one because it indicates the current active key)
	for (i=1; i<TOTAL_CONCURRENT_KEYS-1; i++) {
		if (current_keys[i] == 0) {
			current_keys[i] = current_keys[i+1];
			current_times[i] = current_times[i+1];
			current_keys[i+1] = 0;
			current_times[i+1] = 0;
		}
	}

}


uint32_t active_keycode = 0;

static void process_keyscan(uint32_t keyscan) {
	static uint32_t last_keyscan = 0;
	uint32_t scancode;
	uint32_t pressed  = keyscan & (~last_keyscan);
	uint32_t released = last_keyscan & (~keyscan);

	last_keyscan = keyscan;

	if (pressed && inputhooks && inputhooks->beep) inputhooks->beep();

	for (scancode=1; (released && scancode<32); scancode++) {
		if (released & 1) {
			dispatch_keyrelease(scancode);
			pressedlist_remove(scancode);
			if (active_keycode == scancode)
				active_keycode = 0;
		}
		released >>= 1;
	}

	uint32_t now;
	get_sysmtime(&now);
	for (scancode=1; (pressed && scancode<32); scancode++) {
		if (pressed & 1) {
			dispatch_keypress(scancode);
			pressedlist_add(now,scancode);
			active_keycode = scancode;
			repeatcount = 0;
		}
		pressed >>= 1;
	}
}

void aos_key_management_task(UNUSED void* arg) {
	int i;
	struct extended_char last_scancode;
	unsigned int timedwait = 0;
	
	memset(&last_scancode, 0, sizeof(last_scancode));
	
	while (1) {
		timedwait = 0;
		
		if (active_keycode != 0) {
			if (repeatcount == 0)
					timedwait = 500; // Pre repeation delay
				else
					timedwait = 50; // Repeation delay
		}
		

		if (sem_timeout_down(&keyscan_ready_sem, timedwait) == ETIMEOUT) {
			repeatcount++; // Key is repeated
		} else {
			process_keyscan(current_keyscan);
		}

		if (active_keycode && repeatcount) {
			dispatch_keypress(active_keycode);

			buffer_put_idx++;
			if (buffer_put_idx == BUFFER_SIZE)
				buffer_put_idx = 0;

			for (i=0; i<MAX_CONCURRENT_KEYS; i++) {
				char_buffer[buffer_put_idx].keys[i] = current_keys[i];
				char_buffer[buffer_put_idx].times[i] = current_times[i];
			}
			char_buffer[buffer_put_idx].repeatcount = repeatcount;
	
			if (inputhooks && inputhooks->keyfilter)
				inputhooks->keyfilter(&char_buffer[buffer_put_idx]);

			sem_up(&getchar_ready_sem);
			mutex_unlock(&char_buffer_lock);

			last_scancode.keys[0] = current_keys[0];
		}
	}
}


uint32_t aos_concurrent_keys(const struct extended_char* exchar, const uint32_t keys[MAX_CONCURRENT_KEYS]) {
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
	uint8_t fetchkey = 1;
	//int i;

	memset(&retchar, 0, sizeof(retchar));
	do {
		fetchkey = 0;
		if (timeout >= 0 && sem_timeout_down(&getchar_ready_sem, timeout) == ESUCCESS)
			fetchkey = 1;
			
		if (timeout < 0 && sem_trydown(&getchar_ready_sem))
			fetchkey = 1;
		if (fetchkey) {
			mutex_lock(&char_buffer_lock);
			buffer_get_idx++;
			if (buffer_get_idx == BUFFER_SIZE)
							buffer_get_idx = 0;
			retchar = char_buffer[buffer_get_idx];
			mutex_unlock(&char_buffer_lock);
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

void eventqueue_init(void);

void input_init(void);
void input_init(void)
{
	sem_init(&getchar_ready_sem, 0);
	sem_init(&keyscan_ready_sem, 0);
	eventqueue_init();
}

AOS_MODULE_INIT(input_init);
