/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#ifndef AOS_TYPES_H
#define AOS_TYPES_H

#include <stddef.h>
#include <stdint.h>

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
