/*
		AOS - ARM Operating System
		Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

		This library is free software; you can redistribute it and/or
		modify it under the terms of the GNU Lesser General Public
		License as published by the Free Software Foundation; either
		version 2.1 of the License, or (at your option) any later version.

		This library is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
		Lesser General Public License for more details.

		You should have received a copy of the GNU Lesser General Public
		License along with this library; if not, write to the Free Software
		Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
		*/
#ifndef FRAGMENT_H
#define FRAGMENT_H

#include <aos_module.h>

#include <types.h>

/**
 * \brief The minimum block-size in fragment-stores
 */
#define FRAGMENT_SIZE 32

/**
 * \brief This describes a concept called fragmented storage.
 *
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
