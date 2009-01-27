#ifndef INPUT_H
#define INPUT_H

#define MAX_CONCURRENT_KEYS 3

#ifdef __cplusplus
	extern "C" {
#endif

#define KeyPress   1
#define KeyRelease 2

struct KeyEvent {
	int type;
	unsigned int keycode;
	unsigned int repeatcount;
};

union _AosEvent {
	int type;
	struct KeyEvent keyEvent;
};

typedef union _AosEvent AosEvent;

static inline uint8_t is_event_first_keypress(const AosEvent* e) {
	return (e && (e->type == KeyPress) && (e->keyEvent.repeatcount == 0));
}

int aos_get_event(AosEvent* e, int timeout);

void dispatch_keypress(unsigned int scancode, unsigned int repeatcount);
void dispatch_keyrelease(unsigned int scancode);

/** @todo refine this */
struct extended_char {
	uint32_t keys[MAX_CONCURRENT_KEYS];   // The keys
	uint32_t times[MAX_CONCURRENT_KEYS];  // The time the keys was pressed (not repeated)
	uint32_t repeatcount;
};


struct input_hooks {
	void (*beep)(void);
	void (*keyfilter)(struct extended_char*);
};


/**
 * \brief Normal getchar
 * @return A available character
 */
uint32_t aos_getchar(int timeout);

/**
 * \brief Getchar-function that get extended information like
 * simultanously pressed keys, and when they where pressed.
 * @return The extended key-information
 */
struct extended_char aos_extended_getchar(int timeout);

/**
 * \brief Set the repeatrate 
 * @param hzrate The rate in HZ
 */
void aos_input_setrate(uint32_t hzrate);

/**
 * \brief The delay before keyrepeats
 * @param msdelay Time in milliseconds
 */
void aos_input_setdelay(uint32_t msdelay);

void aos_register_keyscan_irq(uint32_t scancode);

void aos_register_keyscan(uint32_t keyscan);

void aos_register_keypress(uint32_t scancode);

void aos_put_keybuffer(uint32_t key);

void aos_key_management_task(void*);

uint32_t aos_concurrent_keys(const struct extended_char* exchar, const uint32_t keys[MAX_CONCURRENT_KEYS]);

void aos_input_init(const struct input_hooks* hooks);

#ifdef __cplusplus
	}
#endif

#endif
