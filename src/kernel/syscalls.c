/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#define AOS_KERNEL_MODULE

#include <aos/syscalls.h>
#include <aos/aos.h>

/**
	\brief Table containing syscall pointers
*/
void* sys_call_table[] =
{
	[_NR_yield] sys_yield,
	[_NR_msleep] sys_msleep,
	[_NR_mutex_init] sys_mutex_init,
	[_NR_mutex_lock] sys_mutex_lock,
	[_NR_mutex_timeout_lock] sys_mutex_timeout_lock,
	[_NR_mutex_unlock] sys_mutex_unlock,
	[_NR_mutex_trylock] sys_mutex_trylock,
	[_NR_block] sys_block,
	[_NR_unblock] sys_unblock,
	[_NR_malloc] sys_malloc,
	[_NR_free] sys_free,
	[_NR_mmstat] sys_mmstat,
	[_NR_create_task] sys_create_task,
 	[_NR_aos_set_preemptive] sys_aos_set_preemptive,
	[_NR_aos_default_preemptive] sys_aos_default_preemptive,
	[_NR_aos_mm_init] sys_aos_mm_init,
	[_NR_aos_hooks] sys_aos_hooks,
	[_NR_sem_init] sys_sem_init,
	[_NR_sem_up] sys_sem_up,
	[_NR_sem_upn] sys_sem_upn,
	[_NR_sem_down] sys_sem_down,
	[_NR_sem_downn] sys_sem_downn,
 	[_NR_sem_trydown] sys_sem_trydown,
 	[_NR_sem_timeout_down] sys_sem_timeout_down,
	[_NR_get_process_list] sys_get_process_list,
	[_NR_assert_failed] sys_assert_failed,
};

/* Syscall definitions */


_syscall1(void, msleep, uint16_t, ms);
_syscall1(void, unblock, struct task_t*, task);
_syscall1(void, block, struct list_head*, q);
_syscall0(void, yield);
// _syscall0(void, disable_cs);
// _syscall0(void, enable_cs);
_syscall0(struct list_head*, get_process_list);
_syscall4(struct task_t*, create_task, taskFuncPtr, entrypoint, const char*, name, void*, arg, int8_t, priority);
// _syscall0(void, enable_irqs);
// _syscall0(void, disable_irqs);
// _syscall0(void, aos_basic_init);

/* Time-functions */
//_syscall1(void, get_sysutime, uint32_t*, time);
//_syscall1(void, get_sysmtime, uint32_t*, time);

_syscall1(void, aos_set_preemptive, uint8_t, preemptive);
_syscall1(void, aos_default_preemptive, uint8_t, preemptive);


/* Statistics-functions */
_syscall1(void, aos_hooks, struct aos_hooks*, hooks);



