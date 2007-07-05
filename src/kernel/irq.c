#define AOS_KERNEL_MODULE

#include <types.h>
#include <irq.h>
#include <interrupt.h>

struct irq {
	void (*isr)(void);
	uint32_t num_irqs;
};


static volatile uint32_t saved_irq_state;
static volatile uint32_t nr_irq_lock = 0;
uint32_t volatile irq_nest_count = 0;

struct irq irq_table[32];


int irq_attach(int irqnum, void (*isr)(void)) {
	irq_table[irqnum].isr = isr;
	return 0;
}

void irq_handler(int vector) {
	irq_table[vector].num_irqs++;
	//interrupt_enable();
	irq_table[vector].isr();
	//interrupt_disable();
}

void irq_lock(void) {
	uint32_t stat;
	interrupt_save(&stat);
	interrupt_disable();
	if (++nr_irq_lock == 1)
		saved_irq_state = stat;
}

void irq_unlock(void) {
	if (--nr_irq_lock == 0)
		interrupt_restore(saved_irq_state);
}

