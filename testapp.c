#include <string.h>
#include <aos.h>
//#include <arm/lpc2119.h>
#include <arm/lpc23xx.h>
#include <bits.h>
#include <mutex.h>
#include <atomic.h>
//#include <serio.h>
#include <mm.h>
#include <aos_hooks.h>

#define LPC2364 

struct task_t* task1_cd;
struct task_t* task2_cd;
struct task_t* task3_cd;
struct task_t* idle_cd;

void timer_hook(uint32_t time) {
	static int count;
	
	if (count == 50) {
#ifndef LPC2364
		GPIO1_IOPIN ^= BIT24;
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
uint8_t count = 30;

void up(void) {
// 	mutex_lock(&count_lock);
// 	count ++;
// 	mutex_unlock(&count_lock);
}

void AOS_TASK task1(void) {
	for (;;) {
		
		mutex_lock(&mymutex);
		msleep(2);
#ifndef LPC2364
		GPIO1_IOSET = BIT22;
#else
#endif
		mutex_unlock(&mymutex);
		
		up();
		msleep(count);
		
		
		mutex_lock(&mymutex);
#ifndef LPC2364
		GPIO1_IOCLR = BIT22;
#else
#endif
		mutex_unlock(&mymutex);
		
		up();
		msleep(count);
		
	}
}


void AOS_TASK task2(void) {
	for (;;) {
		
		mutex_lock(&mymutex);
		msleep(2);
#ifndef LPC2364
		GPIO1_IOSET = BIT23;
#else
#endif
		mutex_unlock(&mymutex);
		
		up();
		msleep(2*count);
		
		mutex_lock(&mymutex);
#ifndef LPC2364
		GPIO1_IOCLR = BIT23;
#else
#endif
		mutex_unlock(&mymutex);
		
		up();
		msleep(2*count);
	}
}

void AOS_TASK task3(void) {
	for (;;) {
		mutex_lock(&mymutex);
		msleep(2);
#ifndef LPC2364
		GPIO1_IOSET = BIT24;
#else
#endif
		mutex_unlock(&mymutex);

		up();
		msleep(4*count);
		
		mutex_lock(&mymutex);
#ifndef LPC2364
		GPIO1_IOCLR = BIT24;
#else
#endif
		mutex_unlock(&mymutex);
		
		up();
		msleep(4*count);
	}
}


void AOS_TASK task_arr(uint32_t sleeptime) {
	for (;;) {
// 		mutex_lock(&mymutex);
// 		msleep(1);
// 		mutex_unlock(&mymutex);
		msleep(100);
	}
}

void AOS_TASK idle()  {
	for (;;) {
// 		yield();
	}
}



char __attribute__((aligned(4))) dmem[10*1024];

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
#else
	GPIO1_IODIR |= BIT24|BIT23|BIT22;
#endif

	for (;;);
	aos_basic_init();
	aos_mm_init(dmem, dmem+sizeof(dmem));

// 	device_register(&lpcuart);

	
	task1_cd = create_task(task1, NULL, 0);
	task2_cd = create_task(task2, NULL, 0);
// 	task3_cd = create_task(task3, NULL, 0);
	
// 	for (i=0; i<2; i++)
// 		create_task(task_arr, NULL/*(i+2)*2*/, 0);
	
	aos_hooks(&testapp_aos_hooks);
	mutex_init(&mymutex);
	mutex_init(&count_lock);

	aos_context_init(15000000, idle);
}
