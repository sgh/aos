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
#ifndef _MM_H
#define _MM_H

#include <aos/types.h>

#ifdef __cplusplus
	extern "C" {
#endif

/**
 * \brief Dynamic memory statistics
 */
struct mm_stat {
	/**
	 * \brief The amount of overhead for malloc housekeeping.
	 */
	uint16_t overhead;

	/**
	 * \brief The accumulated size of all used memory-segments.
	 */
	uint16_t used;

	/**
	 * \brief The accumulated size of all free memory-segments.
	 */
	uint16_t free;

	/**
	 * \brief The total size of the memory-area used for malloc.
	 */
	uint16_t size;

	/**
	 * \brief The total number of allocations,
	 */
	uint16_t mcount;
};

/**
 * \brief Allocate a piece of memory.
 * @param size The requested size
 * @return Pointer to the memory, or NULL if Out-Of-Memory
 */
void *malloc(size_t size);

/**
 * \brief Free a priviously allocated piece of memory.
 * @param ptr Pointer to the allocated memory to free 
 */
void free(void* ptr);

/**
 * \brief Retrieve memory-allocation statistics.
 * @param stat Pointer to a struct mm_stat where stats should be stored.
 */
void mmstat(struct mm_stat* stat);

/*
 * \brief Specify to the malloc weither is may lock the schdueler.
 * @param allowlock 1 if schedueler may not be locked
 */
void mm_schedlock(uint8_t allowlock);

#ifdef __cplusplus
	}
#endif

#endif
