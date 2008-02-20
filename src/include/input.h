#ifndef INPUT_H
#define INPUT_H

#define MAX_CONCURRENT_KEYS 3

/** @todo refine this */
struct extended_char {
	uint32_t keys[MAX_CONCURRENT_KEYS];   // The keys
	uint32_t times[MAX_CONCURRENT_KEYS];  // The time the keys was pressed (not repeated)
	uint32_t repeatcount;
};


struct input_hooks {
	void (*beep)();
	uint32_t (*keyfilter)(uint32_t);
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

void aos_register_keypress(uint32_t scancode);

void aos_put_keybuffer(uint32_t key);

void aos_key_management_task(void* arg);

uint32_t aos_concurrent_keys(struct extended_char* exchar, uint32_t keys[MAX_CONCURRENT_KEYS]);

void aos_input_init(const struct input_hooks* hooks);

#endif
