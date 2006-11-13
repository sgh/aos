#ifndef FRAGMENT_H
#define FRAGMENT_H

#include <types.h>

#define FRAGMENT_SIZE 32

struct fragment_store {
	struct fragment_store* next;	/**< \brief Pointer to next fragment */
	uint16_t size;						/**< \brief Size of current fragment.
																			If this is the first fragment, the size is
																			the size of the whole line of fragments.
																*/
	uint8_t data[FRAGMENT_SIZE];
};

struct fragment_store* store_fragment(const unsigned char* data, unsigned int size);
void load_fragment(unsigned char* data, struct fragment_store* fragment);
void free_fragment(struct fragment_store* fragment);
void  init_fragment_store();

#endif
