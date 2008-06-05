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
#ifndef _MACROS_H_
#define _MACROS_H_

#include <aos/types.h>
#include <aos/config.h>

// Macros for number-manipulation
#define ciel(val,max)  ((val)>(max) ? (max) : (val))
#define floor(val,min) ((val)<(min) ? (min) : (val))
#define max(a,b) ((a)>(b) ? (a) : (b))

// String manipulation
#define __STRING(x) #x
#define xstr(s) __STRING(s)

// Macros for initcall-sections
#define CLASS_MODULE_INIT(function) void* __attribute__((section(".class_initcalls"))) class_initcall_##function = function;
#define BUS_MODULE_INIT(function) void* __attribute__((section(".bus_initcalls"))) bus_initcall_##function = function;
#define DRIVER_MODULE_INIT(function) void* __attribute__((section(".driver_initcalls"))) driver_initcall_##function = function;
#define AOS_MODULE_INIT(function) void* __attribute__((section(".aos_initcalls"))) aos_initcall_##function = function;

// Structure-calculations
#define container_of(VAL,TYPE,MEMBER)  ((TYPE*)((size_t)VAL - offsetof(TYPE,MEMBER)))

// Time-macros
#define ticks2s(ticks) ((ticks) / HZ)
#define ticks2ms(ticks) (((ticks) * 1000) / HZ)
#define ticks2us(ticks) (((ticks) * 1000000) / HZ)
#define ms2ticks(ms) (((ms) * HZ) / 1000)
#define us2ticks(us) (((us) * HZ) / 1000000)

// Compiler stuff
#define PACKED     __attribute__(( packed ))
#define UNUSED     __attribute__(( unused ))
#define FLATTEN    __attribute__(( flatten ))
#define DEPRECATED __attribute__(( deprecated ))

/**
 * \brief Calculate the difference between two uint32_t
 * Overflow compensated
 * @return the difference
 */
inline static uint32_t uint32diff(uint32_t min, uint32_t max) {
	return min<max ? max-min : (UINT32_MAX - min) + max;
}


#endif // _MACROS_H_
