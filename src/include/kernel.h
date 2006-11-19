#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <macros.h>
#include <task.h>
#include <types.h>
#include <list.h>
#include <syscalls.h>
#include <context.h>
#include <string.h>
#include <semaphore.h>

#define SHARED_STACK

/**
 * \brief The size, in bytes, of a saved process-context.
 */
#define CONTEXT_SIZE

extern uint32_t Top_Stack;

uint32_t __inline__ get_usermode_sp();

uint32_t get_sp();

void set_interrupt_mask(uint8_t i);

void block();
void unblock(struct task_t* task);
void yield();
void msleep(uint16_t ms);
void usleep(uint32_t us);
void disable_cs();
void enable_cs();

extern struct task_t* current;
extern uint8_t do_context_switch;

extern struct list_head readyQ;
extern struct list_head usleepQ;

#endif // _KERNEL_H_
