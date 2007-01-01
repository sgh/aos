#define AOS_KERNEL_MODULE

#include <arm/lpc2119.h>
#include <bits.h>
#include <types.h>
#include <timer.h>
#include <kernel.h>
#include <irq.h>
#include <macros.h>

#define TIMER0_IRQ 4

static int8_t vector_num;

static uint32_t timer_overflows = 0;

void init_timer_interrupt(funcPtr handler, uint32_t timer_refclk) {
	T0_PR = timer_refclk/1000000 - 1;		/* Scale to 1 ms steps */
	T0_PC = 0;													/* Prescale-counter */
	T0_TC = 0xFFA00000;									/* Counter-value */
	T0_MR0 = T0_TC + MAX_TIME_SLICE_US;	/* Match-Register0 */
	
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


uint32_t read_timer32() {
	static uint32_t last_timer = 0;
	register uint32_t new_timer = T0_TC;

 	if (new_timer < last_timer)
		timer_overflows++;

	if (timer_overflows == 2)
		for (;;);

	last_timer = new_timer;
	
	return new_timer;
}


uint64_t read_timer64() {
	uint32_t timerval = read_timer32();
	return timerval + (((uint64_t)timer_overflows)<<32);
}
