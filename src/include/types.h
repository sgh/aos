#ifndef _TYPES_H_
#define _TYPES_H_

#include <stddef.h>
#include <stdint.h>

/* Basic int definitions */
// typedef unsigned char uint8_t;
// typedef signed char int8_t;
// typedef unsigned short indt uint16_t;
// typedef signed short int int16_t;
// typedef long unsigned int uint32_t;
// typedef signed int int32_t;

typedef void (*funcPtr)(void);

/* Derived types */
// typedef uint32 size_t;
typedef uint32_t REGISTER_TYPE;

#ifndef NULL
	#define NULL 0
#endif

#endif // _TYPE_H_
