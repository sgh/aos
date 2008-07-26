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
#ifndef _TYPES_H_
#define _TYPES_H_

#include <stddef.h>
//#include <stdint.h>
#include <inttypes.h>

typedef void (*funcPtr)(void);
typedef void (*taskFuncPtr)(void*);

#ifndef __cplusplus
typedef enum {FALSE=0, TRUE} bool;
#endif

typedef uint32_t REGISTER_TYPE;

#ifndef NULL
	#define NULL ((void*)0)
#endif

#endif // _TYPE_H_
