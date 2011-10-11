/*
		AOS - ARM Operating System
		Copyright (C) 2007  S�ren Holm (sgh@sgh.dk)

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
#ifndef AOS_TYPES_H
#define AOS_TYPES_H

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
	#ifndef __cplusplus
	#define NULL ((void*)0)
	#else
	#define NULL         0
	#endif
#endif

#endif // _TYPE_H_
