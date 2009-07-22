#include <string.h>
#include <aos/aos.h>
#include <arm/lpc23xx.h>
#include <aos/bits.h>
#include <aos/mutex.h>
#include <aos/atomic.h>
#include <aos/mm.h>
#include <aos/irq.h>
#include <aos/aos_hooks.h>
#include <aos/semaphore.h>
#include <aos/syscalls.h>

#include "usbcore.h"
#include "usbdev_lpc23xx.h"

#define AOS_KERNEL_MODULE
#include <aos/interrupt.h>
#define UART0 0

#define UART0_IRQ     0x06        // UART_0 IRQ-vector


char __attribute__((aligned(4))) dmem[7*1024];

semaphore_t empty_sem;
semaphore_t full_sem;
mutex_t lock;

void printbits(unsigned char bits) {
	unsigned int bit = 128;
	while ((U0LSR & BIT5) == 0) ;
	while (bit) {
		
		if (bits & bit)
			U0THR = '1';
		else
			U0THR = '0';
		bit >>= 1;
	}
	U0THR = '\r';
	U0THR = '\n';
}

void printstr(const char* str) {
	int count = 16;

	while (*str) {
		if (count == 16) {
			while ((U0LSR & BIT5) == 0) ;
			count = 0;
		}
		U0THR = *str;
		count++;
		str++;
	}
}

void println() {
	while ((U0LSR & BIT5) == 0) ;
	U0THR = '\r';
	U0THR = '\n';
}

const char const vals[0x10] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
void printhex(unsigned char val) {
	while ((U0LSR & BIT5) == 0) ;
	U0THR = '0';
	U0THR = 'x';
	U0THR = vals[val>>4];
	U0THR = vals[val&0x0F];
	U0THR = ' ';
}

uint32_t packet_buffer[16]; // 64 bytes

static const struct device_descriptor dev_desc = {
	.bLength            = sizeof(struct device_descriptor),
	.bDescriptorType    = DESC_TYPE_DEVICE,
	.bcdUSB             = 0x200,
	.bDeviceClass       = 0xFF,
	.bDeviceSubClass    = 0xFF,
	.bDeviceProtocol    = 0xFF,
	.bMaxPacketSize0    = 64,
	.idVendor           = 0xFFFF,
	.idProduct          = 0x0001,
	.bcdDevice          = 0,
	.iManufacturer      = 0x00,
	.iProduct           = 0x00,
	.iSerialNumber      = 0x00,
	.bNumConfigurations = 1,
};

struct my_configuration_descriptor {
	struct configuration_descriptor conf;
	struct interface_descriptor interface;
	struct endpoint_descriptor out_ep2;
	struct endpoint_descriptor in_ep2;
};

static const struct my_configuration_descriptor conf_desc = {
	.conf = {
		.bLength             = sizeof(struct configuration_descriptor),
		.bDescriptorType     = DESC_TYPE_CONFIGURATION,
		.wTotalLength        = sizeof(struct configuration_descriptor) + sizeof(struct interface_descriptor) + sizeof(struct endpoint_descriptor)*2,
		.bNumInterfaces      = 1,
		.bConfigurationValue = 1,
		.iConfiguration      = 0,
		.bmAttributes        = 0x80,
		.bMaxPower           = 50
	},
	.interface = {
		.bLength             = sizeof(struct interface_descriptor),
		.bDescriptorType     = DESC_TYPE_INTERFACE,
		.bInterfaceNumber    = 0,
		.bAlternateSetting   = 0,
		.bNumEndpoints       = 2,
		.bInterfaceClass     = 0xFF,
		.bInterfaceSubClass  = 0xFF,
		.bInterfaceProtocol  = 0xFF,
		.iInterface          = 0,
	},
	.out_ep2 = {
		.bLength = sizeof(struct endpoint_descriptor),
		.bDescriptorType = DESC_TYPE_ENDPOINT,
		.bEndpointAddress = 0x2,
		.bmAttributes = 0x02,
		.wMaxPacketSize = 64,
		.bInterval = 0,
	},
	.in_ep2 = {
		.bLength = sizeof(struct endpoint_descriptor),
		.bDescriptorType = DESC_TYPE_ENDPOINT,
		.bEndpointAddress = 0x82,
		.bmAttributes = 0x02,
		.wMaxPacketSize = 64,
		.bInterval = 0,
	}
};

void send_device_descriptor(uint8_t ep) {
	write_endpoint(ep, (uint8_t*)&dev_desc, dev_desc.bLength);
}

void send_configuration_descriptor(uint8_t ep) {
	write_endpoint(ep, (uint8_t*)&conf_desc, conf_desc.conf.wTotalLength);
}


void realize_endpoint(uint8_t pEp, int maxpkgsize){
	DEV_INT_CLR = EP_RLZED_INT;
	
	REALIZE_EP |= 1<<pEp;
	EP_INDEX = pEp;
	MAXPACKET_SIZE = maxpkgsize;
	while ((DEV_INT_STAT & EP_RLZED_INT) == 0) ;
	DEV_INT_CLR = EP_RLZED_INT;
}

void parse_control_packet(uint8_t pEp, uint32_t stat) {
	uint8_t lep = pEp >> 1;
	struct setup_packet setup;

	if (! (stat & EP_STAT_STP)) {
		return;
	}
	
	int len = read_endpoint(pEp, &setup, sizeof(packet_buffer) );

	switch (setup.bRequest) {
		case USB_GET_STATUS:
			printstr("U GET_STATUS");
			write_endpoint(lep | BIT7, (uint8_t*)"\0", 1);
			break;
		case USB_CLEAR_FEATURE:
			printstr("U CLEAR_FEATURE");
			write_endpoint(lep | BIT7, 0, 0);
			break;
		case USB_SET_FEATURE:
			printstr("U SET_FEATURE");
			write_endpoint(lep | BIT7, 0, 0);
			break;
		case USB_SET_ADDRESS:
			printstr("SET_ADDRESS ");
			printhex(setup.wValue);
			write_endpoint(lep | BIT7, 0, 0);
			SIE_write(SIE_SET_ADDRESS, (setup.wValue & 0x3F) | BIT7);
			break;
		case USB_GET_DESCRIPTOR:
			printstr("GET_DESCRIPTOR(");
			switch (setup.wValue >> 8) {
				case DESC_TYPE_DEVICE:
					printstr("device");
					send_device_descriptor(lep | BIT7);
					break;
				case DESC_TYPE_CONFIGURATION:
					printstr("configuration");
					send_configuration_descriptor(lep | BIT7);
					break;
				case DESC_TYPE_STRING:
					printstr("U string");
					write_endpoint(lep | BIT7, 0, 0);
					break;
				case DESC_TYPE_INTERFACE:
					printstr("U interface");
					write_endpoint(lep | BIT7, 0, 0);
					break;
				case DESC_TYPE_ENDPOINT:
					printstr("U endpoint");
					write_endpoint(lep | BIT7, 0, 0);
					break;
				case DESC_TYPE_DEVICE_QUALIFIER:
					printstr("U qualifier");
					write_endpoint(lep | BIT7, 0, 0);
					break;
				case DESC_TYPE_OTHER_SPEED_CONFIGURATION:
					printstr("U configuration");
					write_endpoint(lep | BIT7, 0, 0);
					break;
				case DESC_TYPE_INTERFACE_POWER:
					printstr("U power");
					write_endpoint(lep | BIT7, 0, 0);
					break;
				default:
						printhex(setup.wValue>>8);
						printhex(setup.wValue & 0xFF);
						write_endpoint(lep | BIT7, 0, 0);
						break;
			}
			printstr(")");
				
			
			break;
		case USB_SET_DESCRIPTOR:
			printstr("U SET_DESCIPTOR ");
			printhex(setup.wValue);
			break;
		case USB_GET_CONFIGURATION:
			printstr("U GET_CONFIGURATION");
			send_configuration_descriptor(lep | BIT7);
			break;
		case USB_SET_CONFIGURATION:
			printstr("SET_CONFIGURATION ");
			printhex(setup.wValue);
			
			realize_endpoint(4, 64);
			realize_endpoint(5, 64);
			SIE_write(SIE_CONFIGURE_DEVICE, 1);
			
			write_endpoint(lep | BIT7, 0, 0);
			break;
		case USB_GET_INTERFACE:
			printstr("U GET_INTERFACE");
			break;
		case USB_SET_INTERFACE:
			printstr("U SET_INTERFACE");
			printhex(setup.wValue);
			printhex(setup.wIndex);
			break;
		case USB_SYNCH_FRAME:
			printstr("U SYNCH_FRAME");
			break;
		default:
			printstr("UNKNOWN"); break;
			break;
	}
}

void endpoint_input(uint8_t pEp, uint32_t stat) {
	uint8_t lEp = pEp >> 1;
	int len = read_endpoint(pEp, packet_buffer, sizeof(packet_buffer));
// 	static int count = 0;
// 	count++;
// 	write_endpoint(ep | BIT7, 0, 8);
	dataon();
	write_endpoint( lEp | BIT7, packet_buffer, len);
}


void usb_interrupt_handler(UNUSED void* arg) {
	unsigned int dev_intr;
	unsigned int usb_intr;
	unsigned int ep_intr;
	
	usb_intr = USB_INT_STAT;
	ep_intr  = EP_INT_STAT;
	dev_intr = DEV_INT_STAT;

	// Frame interrupt
	if (dev_intr & FRAME_INT) {
		if (dataled) {
			dataled--;
			if (dataled == 0)
				dataoff();
		}
		if (rxled) {
			rxled--;
			if (rxled == 0)
				rxoff();
		}
		if (txled) {
			txled--;
			if (txled == 0)
				txoff();
		}
	}

	// Endpoint interrupt. I don't distinguis between EP_SLOW_INT and EP_FAST_INT
	if (dev_intr & (EP_SLOW_INT|EP_FAST_INT)) {

		for (unsigned char pEp=0; pEp<32; pEp++) {
			
			if (EP_INT_STAT & (1 << pEp)) {
				rxon();
				
				// Clear endpoint receive interrupt and read status
				EP_INT_CLR = 1 << pEp;
				while ((DEV_INT_STAT & CDFULL_INT) == 0) ;
				uint32_t stat = CMD_DATA;

				if ((pEp>>1) == 0) {
					parse_control_packet(pEp, stat);
					println();
				} else
					endpoint_input(pEp, stat);
			}

		}
		USB_CTRL = 0;
	}

	DEV_INT_CLR = dev_intr;

}

void AOS_TASK test1(UNUSED void* arg) {
	uint32_t state = 0;
// 	char buf[128];

	// Power up the USB-controller
	PCONP |= BIT31;

	// Initialize USB clocks
	OTG_CLK_CTRL |= 0x12;
	while ((OTG_CLK_STAT & 0x12) != 0x12) msleep(10);

	// Enable USB pins
	PINSEL1 |= (0x01 << 26); // D+
	PINSEL1 |= (0x01 << 28); // D-
	PINSEL3 |= (0x10 << 28); // Vbus
	PINSEL3 |= (0x01 << 2);  // GoodLink
// 	FIO2DIR |= BIT9;
// 	FIO2CLR = BIT9;

	// Disable pull-ups on Vbus
	PINMODE3 &= ~(0x10 << 28);

	//Set EUBEpin and USBMaxPSize for EP0+EP1 wait for EP_RLZED
	EP_INDEX       = 0;
	MAXPACKET_SIZE = 64;
	EP_INDEX       = 1;
	MAXPACKET_SIZE = 64;
  while ((DEV_INT_STAT & EP_RLZED_INT) == 0);
	
	SIE_write(SIE_SET_ADDRESS, 0x80);
	
	SIE_write(SIE_SET_DEVICE_STATUS, 0x10);

	// Install interrupt handler
	irq_attach(22, usb_interrupt_handler, NULL);

	// Enable USB interrupts
	DEV_INT_EN = DEV_STAT_INT | EP_FAST_INT | EP_SLOW_INT | FRAME_INT;
	EP_INT_EN  = BIT5 | BIT4 | BIT2 | BIT0;
	
// 	EP_INT_EN = 0xFFFFFFFF;

	// Enable interrupt
	interrupt_unmask(22);

	// Pull dowm
	FIO2DIR |= BIT9;
	FIO2CLR = BIT9;

	for (;;) {
		msleep(500);

		state ++;
		if (state & BIT0)
			FIO2SET = BIT6;
		else
			FIO2CLR = BIT6;
// 		unsigned int intr = USB_INT_STAT;
// 		printbits(intr>>3);
	}
}

void my_timer_hook(uint32_t ms) {
	static uint8_t count = 0;
	uint32_t now;

	FIO2SET = BIT7; // Not idle

	count++;
	if (count & BIT4)
		FIO2SET = BIT5;
	else
		FIO2CLR = BIT5;
	
}


struct aos_hooks my_hooks = {
	.timer_event = my_timer_hook,
};


void main(void) {
// 	unsigned char buf[32];
// 	volatile int i;
// 	uint8_t c = 'A';
//	uint32_t baud_rate = 230400;
// 	uint32_t baud_rate = 460800;
 	uint32_t baud_rate = 19200;
	uint32_t pclock = 72000000; /** @todo Not correct */

	PINSEL10 = 0;           /* Disable ETM interface, enable LEDs */
	FIO2DIR  = 0x000000FF;  /* P2.0..7 defined as Outputs         */
	FIO2MASK = 0x00000000;
	FIO2SET  = BIT0;

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
	
// 	U0IER   |= ( BIT0 | BIT1 );  // Enable Receive data available interrupt - set in irq.c,

	aos_basic_init();
	FIO2SET  = BIT1;
	aos_mm_init(dmem, dmem+sizeof(dmem));
	FIO2SET  = BIT2;

	
	aos_hooks(&my_hooks);

	create_task(test1, "test", NULL, 0);

	FIO2SET  = BIT3;
// 	aos_hooks(&my_hooks);

	sem_init(&empty_sem, 0);
	sem_init(&full_sem, 1);
	mutex_init(&lock);

	aos_context_init(pclock/4);
	FIO2PIN = 0x0;

	volatile int j;
	for (;;) {
		
// 		U0THR = 'a';
// 		for (j=0; j<200000; j++) ;
		FIO2CLR = BIT7;
// 		for (j=0; j<200000; j++) ;
// 		FIO2SET = BIT7;
	}
	
}
