#include <assert.h>
#include <aos_hooks.h>
#include <syscalls.h>

_syscall3(void, assert_failed, const char * const, exp, const char* const, file, unsigned int, line);

void sys_assert_failed(const char * const exp, const char* const file, unsigned int line) {
	_aos_hooks->assert_failed(exp, file, line);
}