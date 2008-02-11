#ifndef INPUT_H
#define INPUT_H

/** @todo refine this */
struct extended_char {
	uint32_t key;   // The key
	uint32_t time;  // The time the key was pressed (not repeated)
};


/**
 * \brief Normal getchar
 * @return A available character
 */
uint32_t aos_getchar(void);

/**
 * \brief Getchar-function the get extended information like
 * simultanously pressed keys, and when they where pressed.
 * @return The extended key-information
 */
struct extended_char aos_extended_getchar(void);

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


#endif