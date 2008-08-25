/*
		AOS - ARM Operating System
		Copyright (C) 2007  S�ren Holm (sgh@sgh.dk)

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

#include <aos/aos_module.h>

// #include <kernel.h>
#include <aos/semaphore.h>
#include <aos/task.h>
#include <aos/types.h>
#include <aos/list.h>
#include <aos/mm.h>
#include <aos/aos_hooks.h>
#include <aos/macros.h>


/* Syscall numbers */
#define _NR_yield                  0
#define _NR_test                   1
#define _NR_msleep                 2
#define _NR_usleep                 3
#define _NR_block                  4 
#define _NR_unblock                5
#define _NR_malloc                 6
#define _NR_free                   7
#define _NR_mmstat                 8
#define _NR_create_task            9
#define _NR_aos_mm_init            10
#define _NR_aos_hooks              11
#define _NR_mutex_lock             12
#define _NR_mutex_unlock           13
#define _NR_mutex_trylock          14
#define _NR_mutex_init             15
#define _NR_sem_init               16
#define _NR_sem_up                 17
#define _NR_sem_down               18
#define _NR_user_syscall           19
#define _NR_get_process_list       20
#define _NR_assert_failed          21
#define _NR_mutex_timeout_lock     22
#define _NR_sem_timeout_down       23
#define _NR_sem_trydown            24
#define _NR_sem_upn                25
#define _NR_sem_downn              26

/* Syscall declarations */
struct list_head* sys_get_process_list( void ); // for getting process list to display cpu% of each process on display.
uint32_t sys_user_syscall(uint32_t syscallnr, void* data);
void sys_unblock(struct task_t* task);
void sys_block(struct list_head* q);
void sys_yield(void);
void sys_aos_mm_init(void* start, void* end);
void sys_msleep(uint16_t ms);
void sys_usleep(uint32_t us);
void sys_mutex_init(mutex_t* m);
void sys_mutex_lock(mutex_t* m);
uint8_t sys_mutex_timeout_lock(mutex_t* m, uint32_t timeout);
uint8_t sys_sem_timeout_down(semaphore_t* m, uint32_t timeout);
uint8_t sys_mutex_trylock(mutex_t* m);
void sys_mutex_unlock(mutex_t* m);
void* sys_malloc(size_t size);
void sys_free(void* segment);
void sys_mmstat(struct mm_stat* stat);
struct task_t* sys_create_task(taskFuncPtr entrypoint, const char* name, void* arg, int8_t priority);
void sys_aos_hooks(struct aos_hooks* hooks);

void sys_sem_init(semaphore_t* s, int32_t count);
void sys_sem_down(semaphore_t* s);
uint32_t  sys_sem_downn(semaphore_t* s, uint32_t n);
uint8_t sys_sem_trydown(semaphore_t* s);
void sys_sem_up(semaphore_t* s);
void sys_sem_upn(semaphore_t* s, uint32_t n);

void sys_assert_failed(const char * const exp, const char* const file, unsigned int line);

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
