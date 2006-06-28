#include <arm/lpc2119.h>
#include <types.h>
#include <bits.h>

static uint16 vic_address_map;

int8 vic_request_vector(uint32 func, uint8 irqnum) {
	uint8 i = 0;
	uint32* vector_address = (void*)&VICVectAddr0; // Address of first Vector-Address-Register
	uint32* vector_control = (void*)&VICVectCntl0; // Address of first Vector-Control-Register
	
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


void vic_vector_enable(uint8 vectornum) {
	uint32* vector_control = (void*)&VICVectCntl0; // Address of first Vector-Control-Register
	if (vectornum>15)
		return;
	 vector_control += vectornum;
	 *vector_control |=  BIT5; /* Enable Vector */
}


void vic_vector_disable(uint8 vectornum) {
	uint32* vector_control = (void*)&VICVectCntl0; // Address of first Vector-Control-Register
	if (vectornum>15)
		return;
	 vector_control += vectornum;
	 *vector_control &=  ~BIT5; /* Disable Vector */
}


void vic_irq_enable(uint8 irqnum) {
	if (irqnum>31)
		return;
	VICIntEnable |= BIT0<<irqnum; /* Enable Interrrupt */
}


void vic_irq_disable(uint8 irqnum) {
	if (irqnum>31)
		return;
	VICIntEnable &= ~(BIT0<<irqnum); /* Enable Interrrupt */
}
