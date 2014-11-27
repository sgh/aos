/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#define AOS_KERNEL_MODULE

#include <aos/assert.h>
#include <aos/aos_hooks.h>
#include <aos/syscalls.h>

_syscall3(void, assert_failed, const char * const, exp, const char* const, file, unsigned int, line);

void sys_assert_failed(const char * const exp, const char* const file, unsigned int line) {
	_aos_hooks->assert_failed(exp, file, line);
}
