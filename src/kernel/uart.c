#include <arm/lpc2119.h>
#include <types.h>
#include <bits.h>
#include <semaphore.h>
#include <vic.h>
#include <kernel.h>

/* Interrupt_routine */
void uart0_interrupt();

struct semaphore_t uart0_sem = { .counter = 0 };

struct {
	uint8 buf[8];
	uint8 pidx;
	uint8 gidx;
} uart0_buffer;


void uart0_interrupt_routine() {
	uint32 iir;
	iir = UART0_IIR;
	
	uart0_buffer.buf[uart0_buffer.pidx] = UART0_RBR;
	uart0_buffer.pidx++;
	uart0_buffer.pidx %= sizeof(uart0_buffer.buf);

	GPIO1_IOPIN ^= BIT24;

	sem_V(&uart0_sem);
	
	VICVectAddr = 0; /* Update priority hardware */
}


void uart_init(void) {
// 	uint8 c;
// 	uint32 bit;
	GPIO1_IODIR |= BIT23|BIT22;
	uint8 uart_vector;
	uint32 divisor = 15000000/(38400*16);
	
	// Pinselect to UART0
	PCB_PINSEL0 &= 0xF;
	PCB_PINSEL0 |= 0x5;
	
	UART0_LCR = BIT7 | BIT1 | BIT0;
	
	UART0_DLL = divisor&0xFF;
	UART0_DLM = (divisor>>8)&0xFF;
	
	UART0_FCR = BIT0;
	
	UART0_LCR &= ~BIT7;

	uart_vector = vic_request_vector((uint32)uart0_interrupt,6);
	vic_vector_enable(uart_vector);

	// RXD interrupt
	UART0_IER |= BIT0;

	vic_irq_enable(6);
}

