#include <kernel.h>
#include <arm/lpc2119.h>
#include <bits.h>
#include <timer_interrupt.h>
#include <vic.h>
#include <mutex.h>
#include <atomic.h>
#include <string.h>

struct task_t* current = NULL;
REGISTER_TYPE task1_stack[1024/sizeof(REGISTER_TYPE)];
REGISTER_TYPE task2_stack[1024/sizeof(REGISTER_TYPE)];
REGISTER_TYPE idle_stack[1024/sizeof(REGISTER_TYPE)];

struct task_t task1_cd;
struct task_t task2_cd;
struct task_t idle_cd;


// const uint32 entrypoint_offset = offsetof(struct task_t,entrypoint);
const uint32 stack_offset = offsetof(struct task_t,stack);

uint32 global_int = 0;


mutex_t mymutex;

void task1(void) {
	uint8 c;
	for (;;) {
		mutex_lock(&mymutex);
		mutex_unlock(&mymutex);
		GPIO1_IOSET = BIT22;
		msleep(30);
		GPIO1_IOCLR = BIT22;
		msleep(30);
	}
}

uint32 val = 1;


void task2(void) {
	uint32 old_val = 0;
	uint32 timer = 10000;
	for(;;) {

		if (timer == 10000) {
			old_val = atomic_xchg( &val, old_val);

			if (old_val == 1) mutex_lock(&mymutex);
			msleep(500);
			if (old_val == 1) mutex_unlock(&mymutex);


			if (old_val == 1) {
				GPIO1_IOSET = BIT23;
			} else {			
				GPIO1_IOCLR = BIT23;
			}
			
		}
		
		yield();
	}
}

static void __attribute__((constructor))  init_task(struct task_t* task,funcPtr entrypoint,REGISTER_TYPE* stack) {
	memset((void*)stack,0,1024);
	task->stack = stack;
	task->stack[0] = (uint32)(entrypoint);                                  // Entrypoint
#ifdef SHARED_STACK
	task->stack[1] = (uint32)&Top_Stack;  // Shared stack SP
#else
	task->stack[1] = (uint32)task->stack + (1024 * sizeof(REGISTER_TYPE));  // Seperate stack SP
#endif
	task->stack[2] = 0x12345678;
	task->stack[3] = 0; // r0
	task->stack[4] = 0x60000010;  // SPSR User-mode
	task->stack[5] = 0x1; // r1
	task->stack[6] = 0x2; // r2
	task->stack[7] = 0x3; // r3
	task->stack[8] = 0x4; // r4
	task->stack[9] = 0x5; // r5
	task->stack[10] = 0x6; // r6
	task->stack[11] = 0x7; // r7
	task->stack[12] = 0x8; // r8
	task->stack[13] = 0x9; // r9
	task->stack[14] = 0x10; // r10
	task->stack[15] = 0x11; // r11
	task->stack[16] = 0x12; // r12
	task->state = READY;
	list_push_back(&readyQ,&task->q);
}


void __attribute__((weak)) idle_task()  {
	for (;;) {
	}
}

int main(void) {
	
	init_task(&task1_cd,task1,task1_stack);
	init_task(&task2_cd,task2,task2_stack);
	init_task(&idle_cd,idle_task,idle_stack);
	
	GPIO1_IODIR |= BIT24|BIT23|BIT22;

	init_timer_interrupt();
	enable_timer_interrupt();
	
	T1_PR = 15000000/1000000;		/* Scale to 1 us steps */
	T1_TCR = BIT0;							/* Enable timer0 */
	
	current = NULL;
	__asm__("swi 0");
	
	//i = test(1,2,3,4);
}
