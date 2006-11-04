#include <arm/lpc2119.h>
#include <types.h>
#include <bits.h>
#include <semaphore.h>
#include <vic.h>
#include <kernel.h>
#include <driver_core.h>

#include <platform.h>
#include <serio.h>

/* Interrupt_routine */
void uart0_interrupt();

extern struct platform_device_driver lpcuart_device_driver;
extern struct serio_class_fops lpcuart_serio_fops;

extern struct driver_class_fops lpcuart_serio_driver_fops;

struct semaphore_t uart0_sem;


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


void uart0_interrupt_routine() {
	uint32_t iir;
	char c;
	iir = UART0_IIR;
	
	
	switch (iir&0x0E) {
		case 2: // THRE Interrupt
			if (UART0_LSR&0x20) { // Transmit FIFO empty
				int fifoready = 16;
				while (fifoready-- && get_fifo(&uart0_txbuffer,&c))
					UART0_THR = c;
			}
			break;

		case 4: // RDA Receive data interrupt
			c = UART0_RBR;
			put_fifo(&uart0_rxbuffer,c);
			GPIO1_IOPIN ^= BIT24;
			sem_up(&uart0_sem);
			break;
	}
	
	VICVectAddr = 0; /* Update priority hardware */
}


void lpcuart_init(void) {
// 	uint8 c;
// 	uint32 bit;

	sem_init(&uart0_sem, 0);
	GPIO1_IODIR |= BIT23|BIT22;
	uint8_t uart_vector;
	uint32_t divisor = 15000000/(38400*16);
	
	// Init fifos
	init_fifo(&uart0_rxbuffer);
	init_fifo(&uart0_txbuffer);
	
	// Pinselect to UART0
	PCB_PINSEL0 &= 0xF;
	PCB_PINSEL0 |= 0x5;
	
	UART0_LCR = BIT7 | BIT1 | BIT0;
	
	UART0_DLL = divisor&0xFF;
	UART0_DLM = (divisor>>8)&0xFF;
	
	UART0_FCR = BIT0;
	
	UART0_LCR &= ~BIT7;

	uart_vector = vic_request_vector((uint32_t)uart0_interrupt,6);
	vic_vector_enable(uart_vector);

	// RXD interrupt and THRE interrupt
	UART0_IER |= BIT0|BIT1;

// 	vic_irq_enable(6);
	
	platform_driver_register(&lpcuart_device_driver);
	
	class_fops_register(&serio_class, &lpcuart_serio_driver_fops);
}


static void lpcuart_put(struct device* dev, char* src, size_t len) {
	if (UART0_LSR&0x20) {
		UART0_THR = *src;
		src++;
		len--;
	}
	
	while (len--) {
		put_fifo(&uart0_txbuffer, *src);
		src++;
	}
}


static int lpcuart_probe(struct device* dev) {
	vic_irq_enable(6);
	return 1;
}

static uint8_t uart_ids[] = {1,0};

struct platform_device_driver lpcuart_device_driver = {
	.id_table = uart_ids,
	.driver = {
		.name = "tty driver",
// 		.bus = &platform_bus_type,
		.probe = lpcuart_probe,
// 		.remove = sca61t_remove,
// 		.ioctl = sca61t_ioctl,
	},
};

struct serio_class_fops lpcuart_serio_fops = {
	.put = lpcuart_put,
};

struct driver_class_fops lpcuart_serio_driver_fops = {
	.fops = &lpcuart_serio_fops,
	.driver = &lpcuart_device_driver.driver
};


DRIVER_MODULE_INIT(lpcuart_init);
