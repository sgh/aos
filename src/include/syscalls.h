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
#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <aos_module.h>

// #include <kernel.h>
#include <semaphore.h>
#include <task.h>
#include <types.h>
#include <list.h>
#include <mm.h>
#include <aos_hooks.h>


/* Syscall numbers */
#define _NR_yield          0
#define _NR_test           1
#define _NR_msleep         2
#define _NR_usleep         3
// #define _NR_disable_irqs   4
// #define _NR_enable_irqs    5
#define _NR_block          6
#define _NR_unblock        7
#define _NR_disable_cs     8
#define _NR_enable_cs      9
#define _NR_get_sysutime   10
#define _NR_get_sysmtime   11
#define _NR_malloc         12
#define _NR_free           13
#define _NR_mmstat         14
#define _NR_create_task    15
// #define _NR_aos_basic_init 16
#define _NR_aos_mm_init    17
#define _NR_aos_hooks      18
#define _NR_mutex_lock     19
#define _NR_mutex_unlock   20
#define _NR_mutex_trylock   21

/* Syscall declarations */
void sys_unblock(struct task_t* task);
void sys_block(struct list_head* q);
void sys_yield(void);
void sys_aos_mm_init(void* start, void* end);
void sys_msleep(uint16_t ms);
void sys_usleep(uint32_t us);
void sys_disable_cs(void);
void sys_enable_cs(void);
// void sys_disable_irqs();
// void sys_enable_irqs();
void sys_mutex_lock(mutex_t* m);
uint8_t sys_mutex_trylock(mutex_t* m);
void sys_mutex_unlock(mutex_t* m);
void sys_get_sysutime(uint32_t* time);
void sys_get_sysmtime(uint32_t* time);
void* sys_malloc(size_t size);
void sys_free(void* segment);
void sys_mmstat(struct mm_stat* stat);
// void sys_aos_basic_init(void);
struct task_t* sys_create_task(funcPtr entrypoint, void* arg, int8_t priority);
void sys_aos_hooks(struct aos_hooks* hooks);


#define str(s) #s
#define xstr(s) str(s)

#define __syscallnr(def) xstr(_NR_##def)

/* Syscall-macros */

/* 0-argument syscall */
#define _syscall0(rettype,func) \
rettype func(void) \
{ \
	__asm__ ("swi " __syscallnr(func)); \
}

/* 1-argument syscall */
#define _syscall1(rettype,func,type0,arg0) \
rettype func(type0 arg0) \
{ \
	register uint32_t __r0 __asm__("r0") = (uint32_t)arg0;\
	__asm__ ("swi " __syscallnr(func) : : "r" (__r0) ); \
}

/* 2-argument syscall */
#define _syscall2(rettype,func,type0,arg0,type1,arg1) \
rettype func(type0 arg0,type1 arg1) \
{ \
	register uint32_t __r0 __asm__("r0") = (uint32_t)arg0;\
	register uint32_t __r1 __asm__("r1") = (uint32_t)arg1;\
	__asm__ ("swi " __syscallnr(func) : :"r"(__r0),"r"(__r1) ); \
}

/* 3-argument syscall */
#define _syscall3(rettype,func,type0,arg0,type1,arg1,type2,arg2) \
rettype func(type0 arg0,type1 arg1,type2 arg2) \
{ \
	register uint32_t __r0 __asm__("r0") = (uint32_t)arg0;\
	register uint32_t __r1 __asm__("r1") = (uint32_t)arg1;\
	register uint32_t __r2 __asm__("r2") = (uint32_t)arg2;\
	__asm__ ("swi " __syscallnr(func) : :"r"(__r0),"r"(__r1),"r"(__r2) ); \
}

/* 4-argument syscall */
#define _syscall4(rettype,func,type0,arg0,type1,arg1,type2,arg2,type3,arg3) \
rettype func(type0 arg0,type1 arg1,type2 arg2,type3 arg3) \
{ \
	register uint32_t __r0 __asm__("r0") = (uint32_t)arg0;\
	register uint32_t __r1 __asm__("r1") = (uint32_t)arg1;\
	register uint32_t __r2 __asm__("r2") = (uint32_t)arg2;\
	register uint32_t __r3 __asm__("r3") = (uint32_t)arg3;\
	__asm__ ("swi " __syscallnr(func) : :"r"(__r0),"r"(__r1),"r"(__r2),"r"(__r3)); \
}

#endif // _SYSCALLS_H_
