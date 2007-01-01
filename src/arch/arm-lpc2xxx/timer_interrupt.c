
#include <arm/lpc2119.h>
#include <bits.h>
#include <types.h>
#include <timer_interrupt.h>
#include <kernel.h>
#include <irq.h>
#include <macros.h>

#define TIMER0_IRQ 4

static int8_t vector_num;

void init_timer_interrupt(funcPtr handler, uint32_t timer_refclk) {
	T0_PR = timer_refclk/1000000 - 1;		/* Scale to 1 ms steps */
	T0_PC = 0;													/* Counter-value */
	T0_MR0 = MAX_TIME_SLICE_US;;				/* Match-Register0 */
	
// 	VICVectCntl0 = 4 + BIT5;
// 	VICVectAddr0 = (uint32)timer_interrupt;
	vector_num = request_vector((uint32_t)handler, TIMER0_IRQ);
	
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

// void reset_timer_interrupt() {
// 	T0_TC = 0;
// }

void clear_timer_interrupt() {
	T0_IR = BIT0;    // Clear interrupt
	VICVectAddr = 0; // Update priority hardware
}

uint32_t get_timer_match() {
	return T0_MR0;
}

void set_timer_match(uint32_t matchval) {
	T0_MR0 = matchval;
}

uint32_t read_timer() {
	return T0_TC;
}

