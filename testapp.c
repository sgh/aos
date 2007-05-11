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

#define LPC2364 

struct task_t* task1_cd;
struct task_t* task2_cd;
struct task_t* task3_cd;

void led_irq_start(void) {
// 	GPIO1_IOSET = BIT21;
}

void led_irq_end(void) {
// 	GPIO1_IOCLR = BIT21;
}

void timer_hook(uint32_t time) {
	static int count;
	static char state = 0;
	return;
// 		for (;;) FIO2SET = 0xFFFFFFFF;
	count++;
	if (count == 100) {
		count = 0;
		state ^= 1;
	}
	state = 1;
	if (state)
#ifdef LPC2364
		FIO2SET = BIT1;
#else
		GPIO1_IOSET = BIT23;
#endif
	else
#ifdef LPC2364
		FIO2CLR = BIT1;
#else
		GPIO1_IOCLR = BIT23;
#endif
}

struct aos_hooks testapp_aos_hooks = {
	.timer_event = timer_hook,
};

mutex_t mymutex;

#define SLEEPUNIT 1

mutex_t count_lock;
// int count = 100;

void mswork(uint32_t ms) {
// 	return;
	volatile unsigned int delay;
// 	uint32_t time;
	delay = 10*ms;
	while (delay--);
// 		get_sysmtime(&time);
		
}

void AOS_TASK task1(void) {
	char state = 0;
	for (;;) {
		
		mutex_lock(&mymutex);
		mswork(100000);
// 		if (state)
// 			GPIO1_IOSET = BIT22;
// 		else
// 			GPIO1_IOCLR = BIT22;
		mswork(100000);
		mutex_unlock(&mymutex);

		msleep(200);
		state ^= 1;
	}
}


void AOS_TASK task2(void) {
	
	char state = 0;
	for (;;) {

		mutex_lock(&mymutex);
		mswork(5000);
// 		if (state)
// 			GPIO1_IOSET = BIT23;
// 		else
// 			GPIO1_IOCLR = BIT23;
		mswork(5000);
		mutex_unlock(&mymutex);

		msleep(250);
		
		state ^= 1;
		
	}
}

semaphore_t uart0_rxempty_sem;


struct fifo_buffer {
	uint8_t buf[128];
	uint8_t pidx;
	uint8_t gidx;
	uint8_t num;
};
 
struct fifo_buffer uart0_rxbuffer;

struct fifo_buffer uart0_txbuffer;

static void put_fifo(struct fifo_buffer* fifo, char c) {
	if (fifo->num == sizeof(fifo->buf))
		return;
	fifo->buf[fifo->pidx] = c;
	fifo->pidx++;
	fifo->pidx %= sizeof(fifo->buf);
	fifo->num++;
}

static char get_fifo(struct fifo_buffer* fifo, char* c) {
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

void uart0_isr(void) {
	uint32_t iir;
	char c;
	uint32_t tmp;
	static char state = 1;


// 	while ((iir = U0IIR) & BIT0) {
// 		switch (iir&0x0E) {
// 			case 6: // RLS Receive Line Status
// 				tmp = U0LSR;
// 				tmp = U0RBR;
// 				U0THR = 'A';
// 				break;
// 			case 12: // CTI Character Timerout interrupt
// 			case 4: // RDA Receive data interrupt
				c = U0RBR;
				if (state)
					FIO2SET = BIT2;
				else
					FIO2CLR = BIT2;
				state ^= 1;
				put_fifo(&uart0_rxbuffer,c);
				sys_sem_up(&uart0_rxempty_sem);
// 				U0THR = 'B';
// 				break;
// 			case 2: // THRE Interrupt
// 				U0THR = 'C';
// 				if (U0LSR&0x20) { // Transmit FIFO empty
// 					int fifoready = 16;
// 					while (fifoready-- && get_fifo(&uart0_txbuffer,&c))
// 						U0THR = c;
// 				}
// 				break;
// 		}
// 	}

}

#define UART0_IRQ     0x06        // UART_0 IRQ-vector

void AOS_TASK task3(void) {
	char state;
	char c;
	int i;
	char buf[1024];
	buf [0] = 5;
	uint32_t baud_rate = 19200;
	uint32_t pclock = 72000000;
	
	init_fifo(&uart0_txbuffer);
	init_fifo(&uart0_rxbuffer);
	
	sem_init(&uart0_rxempty_sem, 0);
#ifdef LPC2364
  PCLKSEL0 &= ~( BIT7 | BIT9 );  // just to make sure they are cleared
	PCLKSEL0 |=  ( BIT6 | BIT8 );
#endif

#ifdef LPC2364
		PINSEL0 |=  ( BIT4 | BIT6 );   																// Enable TxD0 and RxD0
		PINSEL0 &= ~( BIT5 | BIT7 );
#endif

		U0FDR   &= ~( BIT0|BIT1|BIT2 | BIT5|BIT6|BIT7); 							// BIT0-2 Fractional divider not used, BIT5-7 MULT
		U0FDR		|=  ( BIT4 );															 						// Mult set to 1

		U0LCR    =  ( BIT0 | BIT1 | BIT7 );             							// 8 bits, no Parity, 1 Stop bit, DLAB=1
		U0DLL    =  (255 & (pclock/(baud_rate*16)));		              // Set baud Rate
		U0DLM    =   ( ( pclock / (baud_rate*16) ) >> 8 );	          // Set baud rate, High divisor latch = 0
		U0LCR   &= ~( BIT7 );                       									// DLAB = 0, access to divisor latch disabled
		
		U0FCR   |=  ( BIT0 ); 																        // BIT0: Enable FIFO

		U0FCR		|=  ( BIT1 | BIT2 | BIT6 | BIT7 );		  							// BIT1-2 Clr rx/tx buffer (Not necessary)
																																	// BIT6-7: 14 characters in rx fifo triggers interrupt
		U0IER	 = ( BIT0 /*| BIT1*/ );  // Enable Receive data available interrupt - set in irq.c,

		irq_attach(UART0_IRQ, uart0_isr);
		interrupt_unmask(UART0_IRQ);
	for (;;) {
		
		if (state)
#ifdef LPC2364
			FIO2SET = BIT7;
#else
			GPIO1_IOSET = BIT23;
#endif
		else
#ifdef LPC2364
			FIO2CLR = BIT7;
#else
			GPIO1_IOCLR = BIT23;
#endif
		msleep(500);
// 		if (buf[0] != 5)
// 			for (;;);
		state ^= 1;
// 		while (!(U0LSR & 0x20)); // not needed if interruptbased with THRE (transmit hold register empty)
// 		U0THR = 'U';
		
		
		sem_down(&uart0_rxempty_sem);
		get_fifo(&uart0_rxbuffer, &c);
// 		for (i=0; i<10; i++)
		put_fifo(&uart0_txbuffer, c);
	}
}


void AOS_TASK task_arr(void) {
	
	for (;;) {
		mutex_lock(&mymutex);	
		mswork(50);
		mutex_unlock(&mymutex);
// 		msleep(1000);
// 		sem_up(&uart0_rxempty_sem);
	}
}

#define TIMER1_IRQ 5

#ifdef US_TIMER
extern uint32_t volatile irq_nest_count;
void test_Handler(void) {
	volatile int test;
	static int count;
	static char state = 0;
// 	for (test=0; test<10; test++);

	T1_IR = BIT0;    // Clear interrupt

// 	if (irq_nest_count > 1)
		count++;

	if (count & BIT0)
			GPIO1_IOSET = BIT22;
	else
		GPIO1_IOCLR = BIT22;
}

void init_timer1(void) {
	T1_PR = 15000000/1000000 - 1;   /* Scale to 1 us steps */
	T1_PC = 0;                          /* Prescale-counter */
	T1_TC = 0;                  /* Counter-value */
	T1_MR0 = 10; /* Match-Register0 */
	irq_attach(TIMER1_IRQ, test_Handler);
	T1_MCR = BIT1|BIT0; /* Interrupt on Math-Register0  reset at match */
	T1_TCR = BIT0;  /* Enable timer1 */

  //VICIntSelect |= ( (uint32_t) 1 ) << t1_vector;
	interrupt_unmask(TIMER1_IRQ);
}
#endif

char __attribute__((aligned(4))) dmem[3*1024];

void main(void) {
	volatile int i;
#ifdef LPC2364
	PINSEL10 = 0;           /* Disable ETM interface, enable LEDs */
	FIO2DIR  = 0x000000FF;  /* P2.0..7 defined as Outputs         */
	FIO2MASK = 0x00000000;
	FIO2CLR  = 0xFF;
#else
	GPIO1_IODIR |= 0xFFFFFFFF;
#endif


// 	GPIO1_IOPIN |= BIT24;
	aos_basic_init();
	
	aos_mm_init(dmem, dmem+sizeof(dmem));
	
// 	device_register(&lpcuart);

	
// 	task1_cd = create_task(task1, NULL, 0);

// 	task2_cd = create_task(task2, NULL, 0);
	task3_cd = create_task(task3, NULL, 0);
	
// 	for (i=0; i<10; i++)
// 		create_task(task_arr, NULL/*(i+2)*2*/, 0);
	
// 	aos_hooks(&testapp_aos_hooks);
	mutex_init(&mymutex);
	mutex_init(&count_lock);
	
	aos_context_init(15000000);
	i = 0;
#ifdef US_TIMER
	init_timer1();
#endif
	for (;;) {
		FIO2CLR = BIT0;
		for (i=0; i<200000; i++);
		FIO2SET = BIT0;
		for (i=0; i<200000; i++);
	}
	
}
