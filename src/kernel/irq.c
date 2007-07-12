#define AOS_KERNEL_MODULE

#include <types.h>
#include <irq.h>
#include <interrupt.h>
#include <assert.h>

#define FIO_BASE_ADDR		0x3FFFC000
#define FIO2SET        (*(volatile unsigned int *)(FIO_BASE_ADDR + 0x58))
#define FIO2CLR        (*(volatile unsigned int *)(FIO_BASE_ADDR + 0x5C))

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

int irq_detach(int irqnum) {
	interrupt_mask(irqnum);
	irq_table[irqnum].isr = NULL;
	return 0;
}

void irqs_are_disabled(void) {
// 	FIO2SET = (1<<3);
}

void irqs_are_enabled(void) {
// 	FIO2CLR = (1<<3);
}

void irq_handler(int vector) {
	ASSERT(irq_table[vector].isr && (vector < 32));
	
	irq_table[vector].num_irqs++;
// 	interrupt_enable();
	irq_table[vector].isr();
// 	interrupt_disable();
}

void irq_lock(void) {
	uint32_t stat;
	interrupt_save(&stat);
	interrupt_disable();
	irqs_are_disabled();
	if ((++nr_irq_lock) == 1)
		saved_irq_state = stat;
}

void irq_unlock(void) {
	if ((--nr_irq_lock) == 0) {
		interrupt_restore(saved_irq_state);
		irqs_are_enabled();
	}
}

