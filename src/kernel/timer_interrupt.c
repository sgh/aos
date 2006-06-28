
#include <arm/lpc2119.h>
#include <bits.h>
#include <types.h>
#include <timer_interrupt.h>
#include <kernel.h>
#include <vic.h>

#define TIMER0_IRQ 4

static int8 vector_num;

void init_timer_interrupt() {
	T0_PR = 15000000/1000;		/* Scale to 1 ms steps */
	T0_MR0 = 1;						/* Match-Register0 */
	
// 	VICVectCntl0 = 4 + BIT5;
// 	VICVectAddr0 = (uint32)timer_interrupt;
	vector_num = vic_request_vector((uint32)timer_interrupt, TIMER0_IRQ);
	
	if ( vector_num ==-1)
		for (;;);
	
	T0_MCR = BIT0 | BIT1;		/* Interrupt on Math-Register0 */
	T0_TCR = BIT0;					/* Enable timer0 */
}


void enable_timer_interrupt() {
	vic_vector_enable(vector_num);
	vic_irq_enable(TIMER0_IRQ);
}

void disable_timer_interrupt() {
	vic_vector_disable(vector_num);
	vic_irq_disable(TIMER0_IRQ);
}

void reset_timer_interrupt() {
	T0_TC = 0;
}


void timer_interrupt_routine() {
	struct task_t* t;
	struct list_head* e;
	uint32 past_time;
	static uint8 count= 0;
	static uint8 onoff = 0;
	 
	T0_IR = BIT0; /* Clear interrupt */
	VICVectAddr = 0; /* Update priority hardware */
	
	if (count == 20) {
		if (onoff)
			GPIO1_IOSET = BIT24;
		else
			GPIO1_IOCLR = BIT24;
		count = 0;
		onoff ^= 1;
	}

	count++;
	
	e =  msleepQ.next;
	while (e) {
		t = get_struct_task(e);
		e = e->next;
		
		past_time = T1_TC;
		if (t->sleep_time) {
			if (t->sleep_time > past_time)
				t->sleep_time -= past_time;
			else
				t->sleep_time = 0;
		}

		if (t->sleep_time == 0) {
			list_erase(&msleepQ,&t->q);
			list_push_front(&readyQ,&t->q);
		}
	}
	
	do_task_switch = 1;
	T1_TC = 0;
}
