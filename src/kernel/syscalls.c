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
#define AOS_KERNEL_MODULE

#include <syscalls.h>
#include <aos.h>

/**
	\brief Table containing syscall pointers
*/
void* sys_call_table[] =
{
	[_NR_yield] sys_yield,
	[_NR_msleep] sys_msleep,
	[_NR_usleep] sys_usleep,
// 	[_NR_disable_irqs] sys_disable_irqs,
// 	[_NR_enable_irqs] sys_enable_irqs,
	[_NR_block] sys_block,
	[_NR_unblock] sys_unblock,
	[_NR_disable_cs] sys_disable_cs,
	[_NR_enable_cs] sys_enable_cs,
	[_NR_get_sysutime] sys_get_sysutime,
	[_NR_get_sysmtime] sys_get_sysmtime,
	[_NR_malloc] sys_malloc,
	[_NR_free] sys_free,
	[_NR_mmstat] sys_mmstat,
	[_NR_create_task] sys_create_task,
	[_NR_aos_basic_init] sys_aos_basic_init,
	[_NR_aos_mm_init] sys_aos_mm_init,
	[_NR_aos_hooks] sys_aos_hooks,
};

/* Syscall definitions */


_syscall1(void, msleep, uint16_t, ms);
_syscall1(void, usleep, uint32_t, us);
_syscall1(void, unblock, struct task_t*, task);
_syscall1(void, block, struct list_head*, q);
_syscall0(void, yield);
_syscall0(void, disable_cs);
_syscall0(void, enable_cs);
// _syscall0(void, enable_irqs);
// _syscall0(void, disable_irqs);
_syscall0(void, aos_basic_init);
_syscall2(void, aos_mm_init, void*,  start, void*, end);
_syscall1(void, get_sysutime, uint32_t*, time);
_syscall1(void, get_sysmtime, uint32_t*, time);
_syscall1(void*, malloc, size_t, size);
_syscall1(void, free, void*, free);
_syscall1(void, mmstat, struct mm_stat*, stat);
_syscall1(void, aos_hooks, struct aos_hooks*, hooks);
_syscall3(struct task_t*, create_task, funcPtr, entrypoint, void*, arg, int8_t, priority);

