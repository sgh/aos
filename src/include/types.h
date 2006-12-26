#ifndef _TYPES_H_
#define _TYPES_H_

#include <stddef.h>
#include <stdint.h>

typedef void (*funcPtr)(void);

typedef uint32_t REGISTER_TYPE;

#ifndef NULL
	#define NULL ((void*)0)
#endif

#endif // _TYPE_H_
