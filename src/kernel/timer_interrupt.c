
#include <arm/lpc2119.h>
#include <bits.h>
#include <types.h>
#include <timer_interrupt.h>
#include <kernel.h>
#include <vic.h>

#define TIMER0_IRQ 4

static int8_t vector_num;

void init_timer_interrupt() {
	T0_PR = 15000000/1000000 - 1;		/* Scale to 1 ms steps */
// 	T0_PC = 0;
	T0_MR0 = 1000;								/* Match-Register0 */
	
// 	VICVectCntl0 = 4 + BIT5;
// 	VICVectAddr0 = (uint32)timer_interrupt;
	vector_num = vic_request_vector((uint32_t)timer_interrupt, TIMER0_IRQ);
	
	if ( vector_num == -1 )
		for (;;);
	
	T0_MCR = BIT0 | BIT1;		/* Reset and Interrupt on Math-Register0 */
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
	uint32_t past_time;
	static uint16_t count= 0;
	static uint8_t onoff = 0;
	uint32_t time_to_wake = 1000;
	
	if (count == 50) { // Do 10 Hz blink
		if (onoff)
			GPIO1_IOSET = BIT24;
		else
			GPIO1_IOCLR = BIT24;
		count = 0;
		onoff ^= 1;
	}
	count++;
	

		// If someone is sleeping
		if (!list_isempty(&msleepQ)) {
			e = list_get_front(&msleepQ);
			t = get_struct_task(e);
	
			past_time = T1_TC;
			if (t->sleep_time) { // If process had time left to sleep
				if (t->sleep_time > past_time)
					t->sleep_time -= past_time;
				else
					t->sleep_time = 0;
			}
	
			if (t->sleep_time == 0) { // If process now has no time left to sleep
				list_erase(/*&msleepQ,*/&t->q);
				list_push_front(&readyQ,&t->q);
			}

			if (!list_isempty(&msleepQ)) {
				e = list_get_front(&msleepQ); // Set e to the next to wake up
				t = get_struct_task(e); // Get task-struct.

				time_to_wake = t->sleep_time;
				if (time_to_wake < 10) /** TODO 10 should be the time of a timerinterrupt times 2 or larger */
					time_to_wake = 10;

				if (time_to_wake > 1000) // This insured at least 100 interrupts pr. sec.
					time_to_wake = 1000;
			}

		}

	T0_MR0 = time_to_wake;
	
	T1_TC = 0;	// Reset delay-timer to zero
	do_context_switch = 1;

	T0_IR = BIT0; /* Clear interrupt */
	VICVectAddr = 0; /* Update priority hardware */
}
