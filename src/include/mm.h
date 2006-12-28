#ifndef _MM_H
#define _MM_H

#include <types.h>

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
};

/**
 * \brief Allocate a piece of memory.
 * @param size The requested size
 * @return Pointer to the memory, or NULL if Out-Of-Memory
 */
void *malloc(size_t size);

/**
 * \brief Free a priviously allocated piece of memory.
 * @param ptr 
 */
void free(void* ptr);

/**
 * \brief Retrieve memory-allocation statistics.
 * @param stat Pointer to a struct mm_stat where stats should be stored.
 */
void mmstat(struct mm_stat* stat);

#endif
