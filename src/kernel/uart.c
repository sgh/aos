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

struct semaphore_t uart0_sem = { .counter = 0 };

struct {
	uint8_t buf[8];
	uint8_t pidx;
	uint8_t gidx;
} uart0_buffer;


void uart0_interrupt_routine() {
	uint32_t iir;
	uint8_t c;
	iir = UART0_IIR;
	
	c = UART0_RBR;
	uart0_buffer.buf[uart0_buffer.pidx] = c;
	uart0_buffer.pidx++;
	uart0_buffer.pidx %= sizeof(uart0_buffer.buf);

	GPIO1_IOPIN ^= BIT24;

	sem_V(&uart0_sem);	
	
	VICVectAddr = 0; /* Update priority hardware */
}


void lpcuart_init(void) {
// 	uint8 c;
// 	uint32 bit;
	GPIO1_IODIR |= BIT23|BIT22;
	uint8_t uart_vector;
	uint32_t divisor = 15000000/(38400*16);
	
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

	// RXD interrupt
	UART0_IER |= BIT0;

// 	vic_irq_enable(6);
	
	platform_driver_register(&lpcuart_device_driver);
	
	class_fops_register(&serio_class, &lpcuart_serio_driver_fops);
}


static void lpcuart_put(struct device* dev, char* src, size_t len) {
	UART0_THR = *src;
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


MODULE_INIT(lpcuart_init);
