#ifndef _MACROS_H_
#define _MACROS_H_

#include <types.h>


// Macros for number-manipulation
#define ciel(val,max)  (val>max ? max : val)
#define floor(val,min) (val<min ? min : val)
#define max(a,b) (a>b ? a : b)

// Macros for initcall-sections
#define CLASS_MODULE_INIT(function) void* __attribute__((section(".class_initcalls"))) class_initcall_##function = function;
#define BUS_MODULE_INIT(function) void* __attribute__((section(".bus_initcalls"))) bus_initcall_##function = function;
#define DRIVER_MODULE_INIT(function) void* __attribute__((section(".driver_initcalls"))) driver_initcall_##function = function;
#define AOS_MODULE_INIT(function) void* __attribute__((section(".aos_initcalls"))) driver_initcall_##function = function;

// Structure-calculations
#define container_of(VAL,TYPE,MEMBER)  ((TYPE*)((size_t)VAL - offsetof(TYPE,MEMBER)))

// Compiler stuff
#define PACKED __attribute__((packed))
#define UNUSED __attribute__((unused))

/**
 * \brief Calculate the difference between two uint32_t
 * Overflow compensated
 * @return the difference
 */
inline static uint32_t uint32diff(uint32_t min, uint32_t max) {
	return min<max ? max-min : (UINT32_MAX - min) + max;
}


#endif // _MACROS_H_
