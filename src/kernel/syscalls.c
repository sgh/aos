#include <syscalls.h>
#include <mm.h>

/**
	\brief Table containing syscall pointers
*/
void* sys_call_table[] =
{
	[_NR_yield] sys_yield,
	[_NR_msleep] sys_msleep,
	[_NR_usleep] sys_usleep,
	[_NR_disable_irqs] sys_disable_irqs,
	[_NR_enable_irqs] sys_enable_irqs,
	[_NR_block] sys_block,
	[_NR_unblock] sys_unblock,
	[_NR_disable_cs] sys_disable_cs,
	[_NR_enable_cs] sys_enable_cs,
	[_NR_get_systime] sys_get_systime,
	[_NR_malloc] sys_malloc,
	[_NR_free] sys_free,
};

/* Syscall definitions */


_syscall1(void, msleep, uint16_t, ms);
_syscall1(void, usleep, uint32_t, us);
_syscall1(void, unblock, struct task_t*, task);
_syscall1(void, block, struct list_head*, q);
_syscall0(void, yield);
_syscall0(void, disable_cs);
_syscall0(void, enable_cs);
_syscall0(void, enable_irqs);
_syscall0(void, disable_irqs);
_syscall1(void, get_systime, uint32_t*, time);
_syscall1(void*, malloc, uint16_t, size);
_syscall1(void,free, void*, free);

