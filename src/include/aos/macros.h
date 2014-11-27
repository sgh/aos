/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#ifndef AOS_MACROS_H
#define AOS_MACROS_H

#include <aos/types.h>

// Macros for number-manipulation
#ifndef __cplusplus
#define max(a,b) ((a)>(b) ? (a) : (b))
#define min(a,b) ((a)<(b) ? (a) : (b))
#endif

// String manipulation
#define __STRING(x) #x
#define xstr(s) __STRING(s)

// Macros for initcall-sections
#define CLASS_MODULE_INIT(function) void* __attribute__((section(".class_initcalls"))) class_initcall_##function = function;
#define BUS_MODULE_INIT(function) void* __attribute__((section(".bus_initcalls"))) bus_initcall_##function = function;
#define DRIVER_MODULE_INIT(function) void* __attribute__((section(".driver_initcalls"))) driver_initcall_##function = function;
#define AOS_MODULE_INIT(function) void* __attribute__((section(".aos_initcalls"))) aos_initcall_##function = (void*)function;

// Structure-calculations
#define container_of(VAL,TYPE,MEMBER)  ((TYPE*)((size_t)VAL - offsetof(TYPE,MEMBER)))

// Time-macros
#define ticks2s(ticks)  ((ticks) / HZ)
#define ticks2ms(ticks) (ticks)                  // (((ticks) * 1000) / HZ)
#define ticks2us(ticks) ((ticks) * 1000)         // (((ticks) * 1000000) / HZ)
#define ms2ticks(ms)    (ms)                     // (((ms) * HZ) / 1000)
#define us2ticks(us)    ((us) / 1000)            // (((us) * HZ) / 1000000)

// Compiler stuff
#ifdef __GNUC__
#define PACKED     __attribute__(( packed ))
#define UNUSED     __attribute__(( unused ))
#define FLATTEN    __attribute__(( flatten ))
#define DEPRECATED __attribute__(( deprecated ))
#define HOT        __attribute__(( hot ))
#endif

#ifdef WIN32
#define PACKED
#define UNUSED
#define FLATTEN
#define DEPRECATED
#define HOT
#endif

#define likely(x)    __builtin_expect(x, 1)
#define unlikely(x)  __builtin_expect(x, 0)

/**
 * \brief Calculate the difference between two uint32_t
 * Overflow compensated
 * @return the difference
 */
__inline static uint32_t uint32diff(uint32_t min, uint32_t max) {
	return min<max ? max-min : (UINT32_MAX - min) + max;
}


#endif // _MACROS_H_
