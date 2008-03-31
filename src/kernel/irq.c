#define AOS_KERNEL_MODULE

#include <types.h>
#include <irq.h>
#include <interrupt.h>
#include <assert.h>
#include <string.h>
#include <syscalls.h>

//#define FIO_BASE_ADDR		0x3FFFC000
//#define FIO2SET        (*(volatile unsigned int *)(FIO_BASE_ADDR + 0x58))
//#define FIO2CLR        (*(volatile unsigned int *)(FIO_BASE_ADDR + 0x5C))

struct irq {
	void (*isr)(void* arg);
	uint32_t num_irqs;
	void* arg;
	uint8_t flags;
};


static volatile uint32_t saved_irq_state;
static volatile uint32_t nr_irq_lock = 0;
uint32_t volatile irq_nest_count = 0;

struct irq irq_table[32];


int irq_attach(int irqnum, void (*isr)(void* arg), void* arg) {
	irq_lock();
	irq_table[irqnum].isr = isr;
	irq_table[irqnum].arg = arg;
	irq_unlock();
	return 0;
}

int irq_detach(int irqnum) {
	irq_lock();
	interrupt_mask(irqnum);
	irq_table[irqnum].isr = NULL;
	irq_table[irqnum].arg = NULL;
	irq_table[irqnum].num_irqs = 0;
	irq_unlock();
	return 0;
}

int irq_flags(int irqnum, unsigned int flags) {
	sys_assert(irqnum < 32);

	irq_lock();
	irq_table[irqnum].flags = flags;
	irq_unlock();
}


uint8_t irq_handler(int vector) {
	uint8_t exclusive = irq_table[vector].flags & IRQ_EXCLUSIVE;

	if (!irq_table[vector].isr)
		return 0;

	if (!exclusive) interrupt_enable();
	irq_table[vector].num_irqs++;
	irq_table[vector].isr(irq_table[vector].arg);
	if (!exclusive) interrupt_disable();
	return 1;
}

void irq_lock(void) {
	uint32_t stat;
	interrupt_save(&stat);
	interrupt_disable();

	if ((++nr_irq_lock) == 1)
		saved_irq_state = stat;
}

void irq_unlock(void) {
	if ((--nr_irq_lock) == 0)
		interrupt_restore(saved_irq_state);
}

static void irq_init(void) {
	memset(irq_table, 0, sizeof(irq_table));
}

AOS_MODULE_INIT(irq_init);
