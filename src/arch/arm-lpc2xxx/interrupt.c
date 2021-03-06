/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#define AOS_KERNEL_MODULE

#include <aos/types.h>
#include <aos/bits.h>
#include <aos/irq.h>
#include <aos/macros.h>
#include <aos/interrupt.h>

#define VICVectAddr0   (*(volatile uint32_t *)0xFFFFF100)

#define VICIntEnable   (*(volatile uint32_t *)0xFFFFF010)
#define VICIRQStatus   (*(volatile uint32_t *)0xFFFFF000)
#define VICDefVectAddr (*(volatile uint32_t *)0xFFFFF034)
#define VICIntEnClr    (*(volatile uint32_t *)0xFFFFF014)


// LPC21xx/LPC22xx registers only
#define VICVectCntl0_LPC21xx   (*(volatile uint32_t *)0xFFFFF200) // LPC21xx/LPC22xx
#define VICVectAddr_LPC21xx    (*(volatile uint32_t *)0xFFFFF030)  // LPC21xx/LPC22xx

// LPC23xx/LPC24xx registers
#define VICVectAddr_LPC23xx    (*(volatile uint32_t *)0xFFFFFF00)  // LPC23xx/LPC24xx

void interrupt_handler(void);

static uint32_t lpc_family = 0;
#define LPC2229_ID  33685267
#define LPC2292_ID  67239699
#define LPC2364A_ID 100924162
#define LPC2364B_ID 369162498
#define LPC2368_ID  100924197

#define IAP_LOCATION 0x7ffffff1
#define REG32 (volatile unsigned int*)

#define T0_IR           (*(REG32 (0xE0004000)))
#define UART0_RBR       (*(REG32 (0xE000C000)))
#define UART0_IIR          (*((volatile unsigned char *) 0xE000C008))
#define UART0_LSR          (*((volatile unsigned char *) 0xE000C014))

extern uint32_t high_priority_irqs;
		
static void detect_lpc_family(void) {
	typedef void (*IAP)(uint32_t [], uint32_t[]);
	uint32_t command[5];
	uint32_t result[2];
	IAP iap_entry;
	iap_entry=(IAP) IAP_LOCATION;

	command[0] = 54;
	iap_entry (command, result);

	switch (result[1]) {
		case LPC2229_ID:
		case LPC2292_ID:
			lpc_family = 2122;
			break;
		case LPC2368_ID:
		case LPC2364A_ID:
		case LPC2364B_ID:
			lpc_family = 2324;
			break;
	}
}

void interrupt_unmask(uint8_t irqnum) {
	volatile uint32_t* vector_addr = &VICVectAddr0;

	if (irqnum>31)
		return;

	irq_lock();
	if (lpc_family == 2324) {
		vector_addr += irqnum;
		*vector_addr = (uint32_t)aos_irq_entry;
	}

	VICIntEnable = (BIT0<<irqnum); /* Enable Interrrupt */
	
	irq_unlock();
}


void interrupt_mask(uint8_t irqnum) {

	if (irqnum>31)
		return;

	VICIntEnClr = (BIT0<<irqnum); /* Disable Interrrupt */
}

void interrupt_init(void) {
	detect_lpc_family();
	VICIntEnClr = 0xFFFFFFFF;
	if (lpc_family == 2122)
		VICDefVectAddr = (uint32_t)aos_irq_entry;
}

void HOT FLATTEN interrupt_handler(void) {
	uint32_t vector;
	uint32_t bits;

	while ((bits = VICIRQStatus)) {
		if (high_priority_irqs & bits)
			bits = bits & high_priority_irqs;

		/** @TODO optimize this */
		for (vector = 0; vector<32; vector++) {
			if (bits & (1<<vector))
				break;
		}

		if (vector < 32)
			irq_handler(vector);
	}

	switch (lpc_family) {
		case 2122: VICVectAddr_LPC21xx = 0; break;
		case 2324: VICVectAddr_LPC23xx = 0; break;
	}
}

