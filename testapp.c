#include <string.h>
#include <aos.h>
#include <arm/lpc2119.h>
// #include <arm/lpc23xx.h>
#include <bits.h>
#include <mutex.h>
#include <atomic.h>
//#include <serio.h>
#include <mm.h>
#include <aos_hooks.h>

// #define LPC2364 

struct task_t* task1_cd;
struct task_t* task2_cd;
struct task_t* task3_cd;

void led_irq_start(void) {
// 	GPIO1_IOSET = BIT21;
}

void led_irq_end(void) {
// 	GPIO1_IOCLR = BIT21;
}

void timer_hook(uint32_t time) {
	static int count;
	static char state = 0;
	
	if (count == 30) {
#ifndef LPC2364
		state ^= 1;
		if (state)
			GPIO1_IOSET = BIT21;
		else
			GPIO1_IOCLR = BIT21;
#else
#endif
		count = 0;
	}
	count++;
}

struct aos_hooks testapp_aos_hooks = {
	.timer_event = timer_hook,
};

mutex_t mymutex;

#define SLEEPUNIT 1

mutex_t count_lock;
int count = 100;

void mswork(uint32_t ms) {
	volatile unsigned int delay;
// 	uint32_t time;
	delay = 10*ms;
	while (delay--);
// 		get_sysmtime(&time);
		
}

void AOS_TASK task1(void) {
	char state = 0;
	for (;;) {
		
// 		mutex_lock(&mymutex);
// 		mswork(10);

		if (state)
			GPIO1_IOSET = BIT22;
		else
			GPIO1_IOCLR = BIT22;

// 		mutex_unlock(&mymutex);

		msleep(500);
		mswork(200000);
		state ^= 1;
	}
}


void AOS_TASK task2(void) {
	int count = 0;
	char state = 0;
	for (;;) {
// 		if (count == 0)
// 			mutex_lock(&mymutex);
// 		mswork(15);
		if (state)
			GPIO1_IOSET = BIT23;
		else
			GPIO1_IOCLR = BIT23;
// 		if (count == 0)
// 			mutex_unlock(&mymutex);

		msleep(500);
		mswork(1000);
// 		yield();
		state ^= 1;
	}
}

void AOS_TASK task3(void) {
	for (;;) {
// 		mutex_lock(&mymutex);
		mswork(25);
// 		mutex_unlock(&mymutex);

// 		msleep(4*count);
		msleep(1000);
		
	}
}


void AOS_TASK task_arr(void) {
	for (;;) {
		for (;;);
		mutex_lock(&mymutex);
		mswork(5);
		mutex_unlock(&mymutex);
		msleep(1000);
	}
}

char __attribute__((aligned(4))) dmem[3*1024];

void main(void) {
	int i;
	//f = sqrtf(f);
#ifdef LPC2364
	PINSEL10 = 0;           /* Disable ETM interface, enable LEDs */
	FIO2DIR  = 0x000000FF;  /* P2.0..7 defined as Outputs         */
	FIO2MASK = 0x00000000;
	FIO2CLR  = 0xFF;

	for (;;) {
		FIO2CLR = 0xFFFFFFFF;
		FIO2SET = 0xFFFFFFFF;
	}
	for (;;);
#else
	GPIO1_IODIR |= 0xFFFFFFFF;
// 	for (;;);
#endif


// 	GPIO1_IOPIN |= BIT24;
	aos_basic_init();
	
	aos_mm_init(dmem, dmem+sizeof(dmem));
	
// 	device_register(&lpcuart);

	
	task1_cd = create_task(task1, NULL, 0);

	task2_cd = create_task(task2, NULL, 0);
// // // 	task3_cd = create_task(task3, NULL, 0);
	
// 	for (i=0; i<1; i++)
// 		create_task(task_arr, NULL/*(i+2)*2*/, 0);
	
	aos_hooks(&testapp_aos_hooks);
	mutex_init(&mymutex);
	mutex_init(&count_lock);
	
	aos_context_init(15000000);
	i = 0;
	for (;;) {
		i++;
/*
		if (i == 100000) {
			GPIO1_IOCLR = BIT21;
		}

		if (i == 200000) {
			GPIO1_IOSET = BIT21;
			i=0;
		}
		*/
	}
	
}
