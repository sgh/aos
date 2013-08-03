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

// pipe is written from ISR
#define PIPE_WR_ISR 1

// pipe is read from ISR
#define PIPE_RD_ISR 2

#define UART0_IRQ     0x06        // UART_0 IRQ-vector

struct fifo_buffer {
	char* buf;
	uint16_t pidx;
	uint16_t gidx;
	uint16_t num;
	uint16_t size;
};

struct pipe {
	
	semaphore_t empty;
	
	semaphore_t full;

	void (*wr_hook)(struct pipe*);

	void (*rd_hook)(struct pipe*);
	
	struct fifo_buffer fifo;
	
	uint8_t mode;
};



char uart0_rxbuffer[32];
char uart0_txbuffer[32];

struct pipe uart0_rx_pipe;
struct pipe uart0_tx_pipe;

static char put_fifo(struct fifo_buffer* fifo, char c) {
	if (fifo->num == fifo->size)
		return 0;
	fifo->buf[fifo->pidx] = c;
	fifo->pidx++;
	fifo->pidx %= fifo->size;
	fifo->num++;
	return 1;
}

static char get_fifo(struct fifo_buffer* fifo, char* c) {
	if (fifo->num == 0)
		return 0;

// 	printf("num: %d\n", fifo->num);
// 	fflush(0);
	*c = fifo->buf[fifo->gidx];
	fifo->gidx++;
	fifo->gidx %= fifo->size;
	fifo->num--;
	return 1;
}

static void init_fifo(struct fifo_buffer* fifo, char* buf,  uint16_t size) {
	memset(fifo,0,sizeof(struct fifo_buffer));
	fifo->buf = buf;
	fifo->size = size;
}


int read_pipe(struct pipe* pipe, char* dst, int len) {
	unsigned int retval = 0;
	unsigned char got_char;
	while (len--) {
		if (! (pipe->mode & PIPE_RD_ISR))
			sem_down(&pipe->empty);
	
		// Get char from buffer
// 		got_char = fifo_read();
		got_char = get_fifo(&pipe->fifo, dst);

		if (!got_char)
			break;

		// sem_post never blocks
		if (pipe->mode & PIPE_RD_ISR)
			sys_sem_up(&pipe->full);
		else
			sem_up(&pipe->full);

		retval++;
		dst++;
	}

	if (pipe->rd_hook)
		pipe->rd_hook(pipe);
	
	return retval;
}


int write_pipe(struct pipe* pipe, char* src, int len) {
	unsigned int retval = 0;
	unsigned char wrote_char;
	
	while (len--) {
		if (! (pipe->mode & PIPE_WR_ISR))
			sem_down(&pipe->full);
	
		// Write char to buffer
		wrote_char = put_fifo(&pipe->fifo, *src);

		if (!wrote_char)
			break;
		
		// sem_post never blocks
		if (pipe->mode & PIPE_WR_ISR)
			sys_sem_up(&pipe->empty);
		else
			sem_up(&pipe->empty);
		
		retval++;
		src++;
	}

	if (pipe->wr_hook)
		pipe->wr_hook(pipe);
	
	return retval;
}

void pipe_init(struct pipe* pipe, char* buf, int size, void (*rd_hook)(struct pipe*), void (*wr_hook)(struct pipe*), uint8_t mode) {
	sem_init(&pipe->empty, 0);
	sem_init(&pipe->full, size);
	pipe->mode = mode;
	pipe->rd_hook = rd_hook;
	pipe->wr_hook = wr_hook;
	init_fifo(&pipe->fifo, buf, size);
}


#define THRE 1
#define RDA 2
#define RLS 3
#define CTI 6

void uart0_wr_hook(struct pipe* pipe) {
	if (U0LSR & BIT5)
		VICSoftInt = UART0_IRQ;
}

void uart0_fill_fifo(struct pipe* pipe) {
	char c;
	uint8_t uart0_thr_slots = 16;
	while (uart0_thr_slots && read_pipe(pipe, &c, 1)) {
		U0THR = c;
		uart0_thr_slots--;
	}

}

void uart0_isr(void* arg) {
	uint32_t iir;
	char c;
// 	uint32_t tmp;
// 	char unlock = 0;
// 	static char state = 1;

	iir = (U0IIR >> 1) & 0x07;
	
	switch (iir) {
		case THRE:
				uart0_fill_fifo(&uart0_tx_pipe);
			break;
		case RLS:
		case CTI:
		case RDA:
			while (U0LSR & BIT0) {
				c = U0RBR;
// 				U0THR = c;
				write_pipe(&uart0_rx_pipe, &c, 1);
// 				put_fifo(&uart0_rxbuffer, c);
// 				sys_sem_up(&uart0_rxempty_sem);
			}
			break;
	}

	if (U0LSR & BIT5)
		uart0_fill_fifo(&uart0_tx_pipe);

	VICSoftIntClr = UART0_IRQ;
	
}


void AOS_TASK uart0_task(void) {
	unsigned int errors;
	unsigned char state = 0;
	char c = 'A';
	char buf[] = "abcdefghijklmnopqrstuvxyz";
// 	buf [0] = 5;
	
	
	for (;;) {
		read_pipe(&uart0_rx_pipe, &c, 1);
		write_pipe(&uart0_tx_pipe, &c, 1);

 		state ^= 1;
// 		if (c != ((last_c+1)&0xFF))
// 			errors++;
// 
 		if (state)
 			FIO2SET = BIT5;
		else
			FIO2CLR = BIT5;
// 		
// 		last_c = c;
// 		msleep(100);
	}
}

void my_timer_hook(uint32_t ms) {
	static uint8_t count = 0;
	uint32_t now;

	FIO2SET = BIT7; // Not idle

	count++;
	if (count & BIT4)
		FIO2SET = BIT0;
	else
		FIO2CLR = BIT0;
	
}


void mcb_assert_failed(const char * const exp, const char* const file, unsigned int line) {
	volatile int i;
	int idx = 0;
	for (;;) {
		FIO2SET = 0xff;
		for (i=0; i<200000; i++);
		FIO2CLR = 0xff;
		for (i=0; i<200000; i++);
		if (file[idx] != 0) {
			U0THR = file[idx];
			idx++;
		}
	}
}

struct aos_hooks my_hooks = {
	.timer_event = my_timer_hook,
	.assert_failed = mcb_assert_failed,
};


char __attribute__((aligned(4))) dmem[7*1024];

void timer1_handler(void) {
	static uint16_t count = 0;
	uint32_t dummy = 0;

	dummy = count;
	if ((count++) == 2048) {
		irq_lock();
		FIO2PIN ^= BIT1;
		irq_unlock();
		count = 0;
	}

	T1IR = BIT0;    // Clear interrupt

}

#define TIMER1_IRQ 5

void AOS_TASK stuff1_task(void) {
	uint8_t buf[1024 + 512];
	while(1) {
		msleep(10);
		
	}
}

void AOS_TASK stuff2_task(void) {
	uint8_t buf[1024 + 512];
	uint32_t time;
	while(1) {
		VICSoftInt = UART0_IRQ;
		msleep(1);
		get_sysmtime(&time);
		msleep(1);
		
	}
}

void init_timer1(void) {
	T1PR = 72000000/1000000 - 1;   /* Scale to 1 us steps */
	T1PC = 0;                          /* Prescale-counter */
	T1TC = 0;                  /* Counter-value */
	T1MR0 = 100; /* Match-Register0 */
	T1MCR = BIT1|BIT0; /* Interrupt on Math-Register0  reset at match */
	T1TCR = BIT0;  /* Enable timer1 */

	irq_attach(TIMER1_IRQ, timer1_handler, NULL);
	interrupt_unmask(TIMER1_IRQ);
}


semaphore_t empty_sem;
semaphore_t full_sem;
mutex_t lock;

void AOS_TASK producer1(void) {
	uint32_t state = 0;
	volatile int i;
	for (;;) {
		sem_down(&full_sem);
		sem_up(&empty_sem);

		state ++;
		if (state & BIT10)
   		FIO2SET = BIT1;
		else
			FIO2CLR = BIT1;

// 		if (state & BIT17) msleep(1);
	}
}

void AOS_TASK producer2(void) {
	uint32_t state = 0;
	for (;;) {
		sem_down(&full_sem);
		sem_up(&empty_sem);

		state ++;
		if (state & BIT10)
			FIO2SET = BIT2;
		else
			FIO2CLR = BIT2;

// 		if (state & BIT17) msleep(1);
	}
}

void AOS_TASK consumer1(void) {
	uint32_t state = 0;
	volatile int i;
	for (;;) {

		mutex_lock(&lock);
		sem_down(&empty_sem);
		sem_up(&full_sem);

		state ++;
		if (state & BIT10)
			FIO2CLR = BIT3;
		else
			FIO2SET = BIT3;

		mutex_unlock(&lock);

// 		if (state & BIT17) msleep(1);
	}
}

void AOS_TASK consumer2(void) {
	uint32_t state = 0;
	volatile int i;
	for (;;) {

		mutex_lock(&lock);

		sem_down(&empty_sem);
		sem_up(&full_sem);
		state ++;
		if (state & BIT10)
			FIO2SET = BIT4;
		else
			FIO2CLR = BIT4;

		mutex_unlock(&lock);

// 		if (state & BIT17) msleep(1);
	}
}

void AOS_TASK test1(void) {
	uint32_t state = 0;
	volatile int i;
	char buf[128];
	
	memset(buf,'u',sizeof(buf));
	for (;;) {
		msleep(50);
		for (i=0; i<sizeof(buf); i++) {
			if (buf[i] != 'u') for (;;) U0THR = buf[i];;
		}
		state ++;
		if (state & BIT0)
			FIO2SET = BIT6;
		else
			FIO2CLR = BIT6;
	}
}

void AOS_TASK test2(void) {
	uint32_t state = 0;
	volatile int i;
	char buf[128];

	memset(buf,'b',128);
	for (;;) {
		for (i=0; i<sizeof(buf); i++)
			if (buf[i] != 'b') for (;;) U0THR = buf[i];;
		mutex_lock(&lock);
		FIO2SET = BIT5;
// 		for (i=0; i<20000; i++) /*yield()*/;
		msleep(30);
		mutex_unlock(&lock);
		FIO2CLR = BIT5;
// 		for (i=0; i<20000; i++) /*yield()*/;
		msleep(30);
	}
	for (;;) {
		msleep(100);
		state ++;
		if (state & BIT0)
			FIO2SET = BIT6;
		else
			FIO2CLR = BIT6;
	}
}

void main(void) {
	volatile int i;
// 	uint8_t c = 'A';
//	uint32_t baud_rate = 230400;
// 	uint32_t baud_rate = 460800;
 	uint32_t baud_rate = 19200;
	uint32_t pclock = 72000000; /** @todo Not correct */

// 	asm ("swi 0");
// 	for (;;);
	
	PINSEL10 = 0;           /* Disable ETM interface, enable LEDs */
	FIO2DIR  = 0x000000FF;  /* P2.0..7 defined as Outputs         */
	FIO2MASK = 0x00000000;
	FIO2SET  = BIT0;

// 	for (;;) {
// 		FIO2SET = BIT5;
// 		for (i=0; i<200000; i++);
// 		FIO2CLR = BIT5;
// 		for (i=0; i<200000; i++);
// 
// 	}

// 	init_timer1();
	
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
// 																																BIT6-7: 14 characters in rx fifo triggers interrupt
	
	U0IER   |= ( BIT0 | BIT1 );  // Enable Receive data available interrupt - set in irq.c,

// 	pipe_init(&uart0_rx_pipe, uart0_rxbuffer, sizeof(uart0_rxbuffer), NULL, NULL, PIPE_WR_ISR);
// 	pipe_init(&uart0_tx_pipe, uart0_txbuffer, sizeof(uart0_txbuffer), NULL, uart0_wr_hook, PIPE_RD_ISR);

// 	irq_attach(UART0_IRQ, uart0_isr);
// 	interrupt_unmask(UART0_IRQ);
	
// 	sem_init(&uart0_rxempty_sem, 0);
// 	sem_init(&uart0_txfull_sem, 128);
	
// 	create_task(uart0_task, "uart0", NULL, 0);
// 	create_task(stuff1_task, "stuff1", NULL, 0);
// 	create_task(stuff2_task, "stuff2", NULL, 0);
	
	aos_basic_init();
	FIO2SET  = BIT1;
	aos_mm_init(dmem, dmem+sizeof(dmem));
	FIO2SET  = BIT2;

	create_task(test1, "test", NULL, 0);
	create_task(test2, "test", NULL, 0);

	create_task(producer1, "p1", NULL, 0);
	create_task(producer2, "p2", NULL, 0);
	create_task(consumer1, "c1", NULL, 15);
	create_task(consumer2, "c2", NULL, 0);

	FIO2SET  = BIT3;
	aos_hooks(&my_hooks);

	sem_init(&empty_sem, 0);
	sem_init(&full_sem, 1);
	mutex_init(&lock);

	aos_context_init(pclock/4);
	FIO2PIN = 0x0;

	for (;;) {
// 		for (i=0; i<20000; i++) sem_down(NULL);
		FIO2CLR = BIT7;
// 		for (i=0; i<20000; i++) sem_down(NULL);
// 		FIO2SET = BIT7;
	}
	
}
