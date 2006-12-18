#ifndef FRAGMENT_H
#define FRAGMENT_H

#include <types.h>

/**
 * \brief The minimum block-size in fragment-stores
 */
#define FRAGMENT_SIZE 32

/**
 * \brief This describes a concept called "fragment-store".
 * It is basically a fragmented distribution of data. It is done
 * because the lack of MMU mean that we may not be able to get 500 bytes
 * continous bytes, whereas the 500 bytes might easily be available in
 * smaller parts
 */
struct fragment_store {
	struct fragment_store* next;	/**< \brief Pointer to next fragment */
	uint16_t size;								/**< \brief Size of current fragment.
																			If this is the first fragment, the size is
																			the size of the whole line of fragments.
																*/
	uint8_t data[FRAGMENT_SIZE];
};

/**
 * \brief Store data in a fragment
 * @param data 
 * @param size 
 * @return 
 */
struct fragment_store* store_fragment(const unsigned char* data, unsigned int size);

/**
 * \brief Restore fragment to memory. The fragment is freed in this process
 * @param data Pointer to where the fragment should be written
 * @param fragment The fragment to restore
 */
void load_fragment(unsigned char* data, struct fragment_store* fragment);

/**
 * \brief Free a fragment. The memory occupied is freed
 * @param fragment The fragment to free
 */
void free_fragment(struct fragment_store* fragment);

#endif
