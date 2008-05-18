#include <aos/assert.h>
#include <aos/aos_hooks.h>
#include <aos/syscalls.h>

_syscall3(void, assert_failed, const char * const, exp, const char* const, file, unsigned int, line);

void sys_assert_failed(const char * const exp, const char* const file, unsigned int line) {
	_aos_hooks->assert_failed(exp, file, line);
}
