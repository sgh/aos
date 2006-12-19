#include <arm/lpc2119.h>
#include <types.h>
#include <bits.h>

static uint16_t vic_address_map;

int8_t request_vector(uint32_t func, uint8_t irqnum) {
	uint8_t i = 0;
	uint32_t* vector_address = (void*)&VICVectAddr0; // Address of first Vector-Address-Register
	uint32_t* vector_control = (void*)&VICVectCntl0; // Address of first Vector-Control-Register
	
	do {
		if ((vic_address_map & (BIT0<<i)) == 0) {
			vic_address_map |= (BIT0<<i);
			break;
		}
		vector_address++;
		vector_control++;
		i++;
	} while (i<16);
	
	if (i==16)
		return -1; // No availabel vector

	*vector_control = irqnum;
	*vector_address = func;

	return i;
}


void vector_enable(uint8_t vectornum) {
	uint32_t* vector_control = (void*)&VICVectCntl0; // Address of first Vector-Control-Register
	if (vectornum>15)
		return;
	 vector_control += vectornum;
	 *vector_control |=  BIT5; /* Enable Vector */
}


void vector_disable(uint8_t vectornum) {
	uint32_t* vector_control = (void*)&VICVectCntl0; // Address of first Vector-Control-Register
	if (vectornum>15)
		return;
	vector_control += vectornum;
	*vector_control &=  ~BIT5; /* Disable Vector */
}


void irq_enable(uint8_t irqnum) {
	if (irqnum>31)
		return;
	VICIntEnable |= BIT0<<irqnum; /* Enable Interrrupt */
}


void irq_disable(uint8_t irqnum) {
	if (irqnum>31)
		return;
	VICIntEnable &= ~(BIT0<<irqnum); /* Enable Interrrupt */
}
