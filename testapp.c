#include <string.h>
#include <aos.h>
//#include <arm/lpc2119.h>
#include <arm/lpc23xx.h>
#include <bits.h>
#include <mutex.h>
#include <atomic.h>
//#include <serio.h>
#include <mm.h>
#include <irq.h>
#include <aos_hooks.h>
#include <semaphore.h>
#include <syscalls.h>

#define AOS_KERNEL_MODULE
#include <interrupt.h>
#define UART0 0

struct fifo_buffer {
	uint8_t buf[128];
	uint8_t pidx;
	uint8_t gidx;
	uint8_t num;
};

semaphore_t uart0_rxempty_sem;
semaphore_t uart0_txfull_sem;

struct fifo_buffer uart0_rxbuffer;
struct fifo_buffer uart0_txbuffer;

static char put_fifo(struct fifo_buffer* fifo, unsigned char c) {
	if (fifo->num == sizeof(fifo->buf))
		return 0;
	fifo->buf[fifo->pidx] = c;
	fifo->pidx++;
	fifo->pidx %= sizeof(fifo->buf);
	fifo->num++;
	return 1;
}

static char get_fifo(struct fifo_buffer* fifo, unsigned char* c) {
	if (fifo->num == 0)
		return 0;
	*c = fifo->buf[fifo->gidx];
	fifo->gidx++;
	fifo->gidx %= sizeof(fifo->buf);
	fifo->num--;
	return 1;
}

static void init_fifo(struct fifo_buffer* fifo) {
	memset(fifo,0,sizeof(struct fifo_buffer));
}


#define THRE 1
#define RDA 2
#define RLS 3
#define CTI 6

void fill_uart0_fifo(void) {
	uint8_t c;
	uint8_t uart0_thr_slots = 16;
	while (uart0_thr_slots && get_fifo(&uart0_txbuffer, &c)) {
		U0THR = c;
		sys_sem_up(&uart0_txfull_sem);
	}
}

void uart0_isr(void) {
	uint32_t iir;
	unsigned char c;
	uint32_t tmp;
	char unlock = 0;
	static char state = 1;

	iir = (U0IIR >> 1) & 0x07;
	
	switch (iir) {
		case THRE:
			fill_uart0_fifo();
			break;
		case RLS:
		case CTI:
		case RDA:
			while (U0LSR & BIT0) {
				c = U0RBR;
				put_fifo(&uart0_rxbuffer, c);
				sys_sem_up(&uart0_rxempty_sem);
			}
			break;
	}
	
}

#define UART0_IRQ     0x06        // UART_0 IRQ-vector

uint8_t uart0_getchar(void) {
	uint8_t c;
	sem_down(&uart0_rxempty_sem);
	interrupt_mask(UART0_IRQ);
	get_fifo(&uart0_rxbuffer, &c);
	interrupt_unmask(UART0_IRQ);
	return c;
}

void uart0_write(uint8_t* src, uint32_t len) {
	while (len--) {
		sem_down(&uart0_txfull_sem);
		put_fifo(&uart0_txbuffer, *src);
		if ((U0LSR & BIT5)) {
			uint8_t c;
			uint8_t uart0_thr_slots = 16;
			while (uart0_thr_slots && get_fifo(&uart0_txbuffer, &c)) {
				U0THR = c;
				sem_up(&uart0_txfull_sem);
			}
		}
		src++;
	}
}


void AOS_TASK uart0_task(void) {
	unsigned int errors;
	unsigned char state;
	unsigned char c = 'A';
	unsigned char last_c = 0xFF;
	char buf[] = "abcdefghijklmnopqrstuvxyz";
// 	buf [0] = 5;
	
	
	for (;;) {
// 		c = uart0_getchar();

		uart0_write(buf, 25);

// 		state ^= 1;
// 		if (c != ((last_c+1)&0xFF))
// 			errors++;
// 
// 		FIO2CLR = 0xFE;
// 		FIO2SET = ((errors<<1) & 0x7E) | state*BIT7;
// 		
// 		last_c = c;
// 		msleep(100);
	}
}



char __attribute__((aligned(4))) dmem[3*1024];

void main(void) {
	volatile int i;
	uint8_t c = 'A';
	uint32_t baud_rate = 230400;
// 	uint32_t baud_rate = 460800;
// 	uint32_t baud_rate = 115200;
	uint32_t pclock = 72000000;
	
	aos_basic_init();
	aos_mm_init(dmem, dmem+sizeof(dmem));
	
	PINSEL10 = 0;           /* Disable ETM interface, enable LEDs */
	FIO2DIR  = 0x000000FF;  /* P2.0..7 defined as Outputs         */
	FIO2MASK = 0x00000000;
	FIO2CLR  = 0xFF;
	
	PCLKSEL0 &= ~( BIT7 | BIT9 );  // just to make sure they are cleared
	PCLKSEL0 |=  ( BIT6 | BIT8 );

	PINSEL0 |=  ( BIT4 | BIT6 );   									// Enable TxD0 and RxD0
	PINSEL0 &= ~( BIT5 | BIT7 );
	
	U0FDR   &= ~( BIT0|BIT1|BIT2 | BIT5|BIT6|BIT7);               // BIT0-2 Fractional divider not used, BIT5-7 MULT
	U0FDR   |=  ( BIT4 );                                         // Mult set to 1

	U0LCR    =  ( BIT0 | BIT1 | BIT7 );                           // 8 bits, no Parity, 1 Stop bit, DLAB=1
	U0DLL    =  (255 & (pclock/(baud_rate*16)));                  // Set baud Rate
	U0DLM    =   ( ( pclock / (baud_rate*16) ) >> 8 );            // Set baud rate, High divisor latch = 0
	U0LCR   &= ~( BIT7 );                                         // DLAB = 0, access to divisor latch disabled

	U0FCR   |=  ( BIT0 );                                         // BIT0: Enable FIFO

	U0FCR   |=  ( BIT1 | BIT2 | BIT6 | BIT7 );                    // BIT1-2 Clr rx/tx buffer (Not necessary)
                                                                    // BIT6-7: 14 characters in rx fifo triggers interrupt
	
	U0IER   |= ( BIT0 | BIT1 );  // Enable Receive data available interrupt - set in irq.c,

	irq_attach(UART0_IRQ, uart0_isr);
	interrupt_unmask(UART0_IRQ);

	init_fifo(&uart0_txbuffer);
	init_fifo(&uart0_rxbuffer);
	
	sem_init(&uart0_rxempty_sem, 0);
	sem_init(&uart0_txfull_sem, 128);
	
	create_task(uart0_task, "uart0", NULL, 0);

	aos_context_init(15000000);
	for (;;) {
		FIO2CLR = BIT0;
		for (i=0; i<200000; i++);
		FIO2SET = BIT0;
		for (i=0; i<200000; i++);

	}
	
}
