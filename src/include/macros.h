#ifndef _MACROS_H_
#define _MACROS_H_

#include <types.h>

#define container_of(VAL,TYPE,MEMBER)  ((TYPE*)((size_t)VAL - offsetof(TYPE,MEMBER)))

/**
 * \brief Calculate the difference between two uint32_t
 * Overflow compensated
 * @return the difference
 */
inline static uint32_t uint32diff(uint32_t min, uint32_t max) {
	return min<max ? max-min : max + UINT32_MAX - min;
}


#endif // _MACROS_H_
