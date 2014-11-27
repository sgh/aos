/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#ifndef AOS_RUNTIME_CHECK_H
#define AOS_RUNTIME_CHECK_H

#include <aos/aos_module.h>
#include <aos/bits.h>

#define STACK_USR BIT0
#define STACK_IRQ BIT1
#define STACK_SVC BIT2

void init_runtime_check(void);
void check_stack(void);

#endif
