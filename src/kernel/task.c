#include <string.h>
#include <mm.h>

#include <kernel.h>
#include <task.h>
#include <types.h>

struct task_t* create_task(funcPtr entrypoint, int8_t priority) {
	struct task_t* t;
	t = malloc(sizeof(struct task_t));
	init_task(t, entrypoint, priority);
	return t;
}

void init_task(struct task_t* task,funcPtr entrypoint, int8_t priority) {
	REGISTER_TYPE cpsr = 0x00000010; // User-mode
	if (((uint32_t)entrypoint & 1) == 1) // If address is thumb
		cpsr |= 0x20;	// Set thumb bit
// 	memset( (void*)stack, 0, 64);
	memset(task, 0, sizeof(struct task_t));
	task->context = malloc(sizeof(REGISTER_TYPE) * 17);
// 	task->fragment = NULL;
	task->context[0] = (uint32_t)(entrypoint);                                  // Entrypoint
#ifdef SHARED_STACK
	task->context[1] = (uint32_t)&Top_Stack;  // Shared stack SP
#else
	task->context[1] = (uint32_t)task->context + (1024 * sizeof(REGISTER_TYPE));  // Seperate stack SP
#endif
	task->context[2] = 0x12345678; // LR
	task->context[3] = 0; // r0
	task->context[4] = cpsr;  // SPSR 
	task->context[5] = 0x1; // r1
	task->context[6] = 0x2; // r2
	task->context[7] = 0x3; // r3
	task->context[8] = 0x4; // r4
	task->context[9] = 0x5; // r5
	task->context[10] = 0x6; // r6
	task->context[11] = 0x7; // r7
	task->context[12] = 0x8; // r8
	task->context[13] = 0x9; // r9
	task->context[14] = 0x10; // r10
	task->context[15] = 0x11; // r11
	task->context[16] = 0x12; // r12
	task->state = READY;
	task->priority = priority;
	list_push_back(&readyQ,&task->q);
}
