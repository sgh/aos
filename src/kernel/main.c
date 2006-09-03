#include <string.h>
#include <kernel.h>
#include <arm/lpc2119.h>
#include <bits.h>
#include <timer_interrupt.h>
#include <vic.h>
#include <mutex.h>
#include <atomic.h>

struct task_t* current = NULL;
REGISTER_TYPE task1_stack[1024/sizeof(REGISTER_TYPE)];
REGISTER_TYPE task2_stack[1024/sizeof(REGISTER_TYPE)];
REGISTER_TYPE idle_stack[1024/sizeof(REGISTER_TYPE)];

struct task_t task1_cd;
struct task_t task2_cd;
struct task_t idle_cd;


// const uint32 entrypoint_offset = offsetof(struct task_t,entrypoint);
const uint32 context_offset = offsetof(struct task_t,context);

uint32 global_int = 0;


mutex_t mymutex;

void /*__attribute__((noreturn)) __attribute__((nothrow))*/ task1(void) {
// 	uint8 c;
	char a[500];
	int i;

	for (;;) {
		
		for (i=0; i<sizeof(a); i++)
			a[i] = '1';

		for (i=0; i<sizeof(a); i++)
			if (a[i] != '1')
				for (;;);
		
		mutex_lock(&mymutex);
		mutex_unlock(&mymutex);
		GPIO1_IOSET = BIT22;
		msleep(30);
		GPIO1_IOCLR = BIT22;
		msleep(30);
	}
}

uint32 val = 1;

void /*__attribute__((noreturn)) __attribute__((nothrow))*/ task2(void) {
	uint32 old_val = 0;
	uint32 timer = 10000;
	char a[500];
	int i;
	
	for(;;) {
		
		for (i=0; i<sizeof(a); i++)
			a[i] = '2';

		for (i=0; i<sizeof(a); i++)
			if (a[i] != '2')
				for (;;);
			
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

static void init_task(struct task_t* task,funcPtr entrypoint,REGISTER_TYPE* stack) {
	memset((void*)stack,0,1024);
	task->context = stack;
	task->context[0] = (uint32)(entrypoint);                                  // Entrypoint
#ifdef SHARED_STACK
	task->context[1] = (uint32)&Top_Stack;  // Shared stack SP
#else
	task->context[1] = (uint32)task->context + (1024 * sizeof(REGISTER_TYPE));  // Seperate stack SP
#endif
	task->context[2] = 0x12345678; // LR
	task->context[3] = 0; // r0
	task->context[4] = 0x60000010;  // SPSR User-mode
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
	list_push_back(&readyQ,&task->q);
}


void /*__attribute__((weak)) __attribute__((noreturn)) __attribute__((nothrow))*/ idle_task()  {
	for (;;) {
	}
}

// Linker provides theese
extern void __start_initcalls;
extern void __stop_initcalls;

void do_initcalls() {
	void (*initcall)();
	
	initcall = &__start_initcalls;
	while (initcall != &__stop_initcalls) {
		initcall();
		initcall++;
	}

	// Init classes
// 	inclitiometer_class_init();

	// Init busses
// 	init_spi();
	
	// Init drivers
// 	sca61t_init();
}


char __attribute__((aligned(4))) dmem[4*1024];

void mm_init(void* start, unsigned short len);

int /*__attribute__((noreturn)) __attribute__((nothrow))*/  main(void) {
	
	mm_init(dmem, sizeof(dmem));
	
	do_initcalls();
	
	init_task(&task1_cd,task1,task1_stack);
	init_task(&task2_cd,task2,task2_stack);
	init_task(&idle_cd,idle_task,idle_stack);
	
	GPIO1_IODIR |= BIT24|BIT23|BIT22;
	
	mutex_init(&mymutex);
	
	init_timer_interrupt();
	enable_timer_interrupt();
	
	T1_PR = 15000000/1000000;		/* Scale to 1 us steps */
	T1_TCR = BIT0;							/* Enable timer0 */
	
	current = NULL;
	__asm__("swi 0");
	
	//i = test(1,2,3,4);
}
