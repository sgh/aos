#include <syscalls.h>

/**
	\brief Table containing syscall pointers
*/
void* sys_call_table[] =
{
	[_NR_yield] sys_yield,
	[_NR_msleep] sys_msleep,
	[_NR_usleep] sys_usleep,
	[_NR_sem_down] sys_sem_down,
	[_NR_sem_up] sys_sem_up,
	[_NR_block] sys_block,
	[_NR_unblock] sys_unblock,
};

/* Syscall definitions */


_syscall1(void,msleep, uint16_t, ms);
_syscall1(void,usleep, uint16_t, us);
_syscall1(void,unblock, struct task_t*, task);
_syscall1(void,block, struct list_head*, q);
_syscall0(void,yield);

