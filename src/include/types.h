#ifndef _TYPES_H_
#define _TYPES_H_

/* Basic int definitions */
typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned short int uint16;
typedef signed short int int16;
typedef long unsigned int uint32;
typedef signed int int32;

typedef void (*funcPtr)(void);

/* Derived types */
typedef uint32 size_t;
typedef uint32 REGISTER_TYPE;

#ifndef NULL
	#define NULL 0
#endif

#endif // _TYPE_H_
