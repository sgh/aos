#include <string.h>
#include <kernel.h>
#include <arm/lpc2119.h>
#include <bits.h>
#include <timer_interrupt.h>
#include <vic.h>
#include <mutex.h>
#include <atomic.h>
#include <driver_core.h>
#include <platform.h>
#include <serio.h>
#include <mm.h>

struct task_t* task1_cd;
struct task_t* task2_cd;
struct task_t* idle_cd;

mutex_t mymutex;


void /*__attribute__((noreturn)) __attribute__((nothrow))*/ task1(void) {
// 	uint8 c;
// 	char a[500];
	int i;
	int p;
	char* ptr;
	
	for (;;) {
		
// 		for (i=0; i<sizeof(a); i++)
// 			a[i] = '1';

// 		for (i=0; i<sizeof(a); i++)
// 			if (a[i] != '1')
// 				for (;;);
		
		mutex_lock(&mymutex);
		mutex_unlock(&mymutex);
		GPIO1_IOSET = BIT22;
		msleep(75);
		GPIO1_IOCLR = BIT22;
//  		disable_cs();
// 		

 		disable_irqs();
		
		i = 0;
		p = 0;
		while (p<200000) {
			i++;
			p++;
			if (p != i) {
				disable_cs();
				for (;;);
			}
		}
		enable_irqs();
// 		enable_cs();

		ptr = malloc(200);
		msleep(75);
		free(ptr);
	}
}

uint32_t val = 1;


void /*__attribute__((noreturn)) __attribute__((nothrow))*/ task2(void) {
	uint32_t old_val = 0;
	uint32_t timer = 10000;
	char a[500];
	char buf[40];
	struct serio console;
	int i;
	char* ptr;
	
// 	acquire_serio(0,&console);
	
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
			
// 			snprintf(buf,sizeof(buf),"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef\r\n");
// 			serio_put(&console,buf,strlen(buf));
			
			if (old_val == 1) mutex_unlock(&mymutex);


			if (old_val == 1) {
				GPIO1_IOSET = BIT23;
			} else {
				GPIO1_IOCLR = BIT23;
			}
		}
		ptr = malloc(200);
		yield();
		free(ptr);
	}
}


void /*__attribute__((weak)) __attribute__((noreturn)) __attribute__((nothrow))*/ idle_task()  {
	uint8_t onoff = 0;
	uint32_t count = 0;
	for (;;) {
// 		if (count == 0xFFFFF) {
// 			if (onoff)
// 				GPIO1_IOSET = BIT24;
// 			else
// 				GPIO1_IOCLR = BIT24;
// 			count = 0;
// 			onoff ^= 1;
// 		}
// 		onoff ^= 1;
// 		onoff ^= 1;
//  		//yield();
// 		count++;
	}
}



char __attribute__((aligned(4))) dmem[5*1024];

struct device lpcuart = {
	.name = "lpcuart",
	.bus = &platform_bus_type,
	.device_id = 1,
};

void main(void) {
	GPIO1_IODIR |= BIT24|BIT23|BIT22;
	
	aos_basic_init();
	
	mm_init(dmem, sizeof(dmem));
	
	device_register(&lpcuart);
	
	task1_cd = malloc(sizeof(struct task_t));
	task2_cd = malloc(sizeof(struct task_t));
	idle_cd = malloc(sizeof(struct task_t));

	init_task(task1_cd,task1, 0);
	init_task(task2_cd,task2, 0);
	init_task(idle_cd,idle_task, 0);
	
	mutex_init(&mymutex);
	
	aos_context_init();
}
