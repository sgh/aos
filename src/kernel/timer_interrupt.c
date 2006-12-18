
#include <arm/lpc2119.h>
#include <bits.h>
#include <types.h>
#include <timer_interrupt.h>
#include <kernel.h>
#include <irq.h>

#define TIMER0_IRQ 4

static int8_t vector_num;
uint32_t last_interrupt_time = 0; /** \brief Timer value at last interrupt */

void init_timer_interrupt() {
	T0_PR = 15000000/1000000 - 1;		/* Scale to 1 ms steps */
	T0_PC = 0;										/* Counter-value */
	T0_MR0 = 1000;								/* Match-Register0 */
	
// 	VICVectCntl0 = 4 + BIT5;
// 	VICVectAddr0 = (uint32)timer_interrupt;
	vector_num = request_vector((uint32_t)timer_interrupt, TIMER0_IRQ);
	
	if ( vector_num == -1 )
		for (;;);
	
	T0_MCR = BIT0;	/* Interrupt on Math-Register0 */
	T0_TCR = BIT0;	/* Enable timer0 */
}


void enable_timer_interrupt() {
	vector_enable(vector_num);
	irq_enable(TIMER0_IRQ);
}

void disable_timer_interrupt() {
 	vector_disable(vector_num);
	irq_disable(TIMER0_IRQ);
}

void reset_timer_interrupt() {
	T0_TC = 0;
}

void sys_get_systime(uint32_t* time) {
	if (time != NULL)
		*time = T0_TC;
}

uint32_t get_interrupt_elapsed() {
	register uint32_t now = T0_TC;
	return now>=last_interrupt_time ? now-last_interrupt_time: 0xFFFFFFFF - (last_interrupt_time-now);
}

void timer_interrupt_routine() {
	struct task_t* t;
	struct list_head* e;
	uint32_t elapsed_time;
	static uint16_t count= 0;
	static uint8_t onoff = 0;
	uint32_t time_to_wake = 1000;

	// If a one process is waiting, do context_switch
	if (!list_isempty(&readyQ))
		do_context_switch = 1; // Signal context-switch
	
	// If someone is sleeping
	if (!list_isempty(&usleepQ)) {
		e = list_get_front(&usleepQ);
		t = get_struct_task(e);

		elapsed_time = get_interrupt_elapsed();
		if (t->sleep_time) { // If process had time left to sleep
			if (t->sleep_time > elapsed_time)
				t->sleep_time -= elapsed_time;
			else
				t->sleep_time = 0;
		}

		if (t->sleep_time == 0) { // If process now has no time left to sleep
			struct task_t* next = get_struct_task(list_get_front(&readyQ));
			list_erase(&t->q);
			list_push_front(&readyQ,&t->q);
 			if (t->priority > next->priority)
				do_context_switch = 0; // Un-signal context-switch
		}

		if (!list_isempty(&usleepQ)) {
			e = list_get_front(&usleepQ); // Set e to the next to wake up
			t = get_struct_task(e); // Get task-struct.

			time_to_wake = t->sleep_time;
			if (time_to_wake < 50) /** TODO 10 should be the time of a timerinterrupt times 2 or larger */
				time_to_wake = 50;

			if (time_to_wake > 1000) // This insures at least 1000 interrupts pr. sec.
				time_to_wake = 1000;
		}

	}

	if (count == 50) { // Do 10 Hz blink
		if (onoff)
			GPIO1_IOSET = BIT24;
		else
			GPIO1_IOCLR = BIT24;
		count = 0;
		onoff ^= 1;
	}
	count++;

// 	if (do_context_switch)
// 		GPIO1_IOPIN ^= BIT24;
// 	else
// 		GPIO1_IOCLR = BIT24;

	sys_get_systime(&last_interrupt_time);

	T0_MR0 = last_interrupt_time + time_to_wake;

	T0_IR = BIT0; /* Clear interrupt */
	VICVectAddr = 0; /* Update priority hardware */
}
