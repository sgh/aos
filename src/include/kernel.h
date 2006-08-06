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

extern uint32 Top_Stack;

uint32 get_usermode_sp();

uint32 get_sp();

void set_interrupt_mask(uint8 i);

void block();
void unblock(struct task_t* task);
void yield();
void msleep(uint16 ms);
void usleep(uint16 us);
void sem_P(struct semaphore_t* s);
void sem_V(struct semaphore_t* s);

extern struct task_t* current;
extern uint8 do_context_switch;

extern struct list_head readyQ;
extern struct list_head readyQ;
extern struct list_head msleepQ;
extern struct list_head usleepQ;

#endif // _KERNEL_H_
