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


void SIE_cmd(uint8_t cmd) {
	DEV_INT_CLR = CDFULL_INT | CCEMPTY_INT;

	// Write the command to the CMD_CODE register
	CMD_CODE = SIE_COMMAND_PHASE | (cmd << 16);
	while ((DEV_INT_STAT & CCEMPTY_INT) == 0) ;
	DEV_INT_CLR = CCEMPTY_INT;
}

void SIE_write(uint8_t cmd, uint16_t data) {
	SIE_cmd(cmd);

	// Write command data
	CMD_CODE = SIE_WRITE_PHASE | (data << 16);
	while ((DEV_INT_STAT & CCEMPTY_INT) == 0) ;
	DEV_INT_CLR = CCEMPTY_INT;
}

void SIE_read(uint8_t cmd, uint8_t* data/*,int len = 1*/) {
	SIE_cmd(cmd);

	// Read data
	CMD_CODE = SIE_READ_PHASE | (cmd << 16);
	while ((DEV_INT_STAT & CDFULL_INT) == 0) ;
	DEV_INT_CLR = CDFULL_INT;
	*data = CMD_DATA;
}



/** convert from endpoint address to endpoint index */
#define EP2IDX(bEP) ((((bEP)&0xF)<<1)|(((bEP)&0x80)>>7))
/** convert from endpoint index to endpoint address */
#define IDX2EP(idx) ((((idx)<<7)&0x80)|(((idx)>>1)&0xF))

uint8_t rxled;
uint8_t txled;

#define txon() {FIO2SET = BIT1; txled = 10; }
#define rxon() {FIO2SET = BIT0; rxled = 10; }

#define txoff() {FIO2CLR = BIT1; }
#define rxoff() {FIO2CLR = BIT0; }

void write_endpoint(uint8_t ep, uint8_t* data, int len) {
	txon();
	
	USB_CTRL   = DEV_WRITE(ep);
	
	TX_PLENGTH = len;

// 	U0THR = '<';
	while (USB_CTRL & WR_EN) {
		TX_DATA = (data[3]<<24) | (data[2]<<16) | (data[1]<<8) | (data[0]);
// 		printhex(data[0]);
// 		printhex(data[1]);
// 		printhex(data[2]);
// 		printhex(data[3]);
		data += 4;
	}
// 	U0THR = '>';

	USB_CTRL = 0;

	SIE_cmd(EP2IDX(ep));
	SIE_cmd(SIE_VALIDATE_BUFFER);

}

void send_device_descriptor(uint8_t ep) {
	struct device_descriptor desc;
	
	desc.bLength            = sizeof(struct device_descriptor);
	desc.bDescriptorType    = DESC_TYPE_DEVICE;
	desc.bcdUSB             = 0x200;
	desc.bDeviceClass       = 0;
	desc.bDeviceSubClass    = 0;
	desc.bDeviceProtocol    = 0;
	desc.bMaxPacketSize0    = 64;
	desc.idVendor           = 0xFEDE;
	desc.idProduct          = 0xCAFE;
	desc.bcdDevice          = 0;
	desc.iManufacturer      = 0;
	desc.iProduct           = 0;
	desc.iSerialNumber      = 0;
	desc.iNumConfigurations = 1;

	write_endpoint(ep, (uint8_t*)&desc, desc.bLength);
}

#define USB_GET_STATUS         0
#define USB_CLEAR_FEATURE      1
#define USB_SET_FEATURE        3
#define USB_SET_ADDRESS        5
#define USB_GET_DESCRIPTOR     6
#define USB_SET_DESCRIPTOR     7
#define USB_GET_CONFIGURATION  8
#define USB_SET_CONFIGURATION  9
#define USB_GET_INTERFACE      10
#define USB_SET_INTERFACE      11
#define USB_SYNCH_FRAME        12


void parse_packet(uint8_t ep, UNUSED  int len, UNUSED uint32_t stat) {
	struct setup_packet setup;
	memcpy(&setup, packet_buffer, sizeof(struct setup_packet));

	if (! (stat & BIT2))
		return;

	switch (setup.bRequest) {
		case USB_GET_STATUS:
			printstr("U GET_STATUS");
			write_endpoint(ep | BIT7, 0, 0);
			break;
		case USB_CLEAR_FEATURE:
			printstr("U CLEAR_FEATURE");
			break;
		case USB_SET_FEATURE:
			printstr("U SET_FEATURE");
			break;
		case USB_SET_ADDRESS:
			printstr("SET_ADDRESS ");
			printhex(setup.wValue);
			write_endpoint(ep | BIT7, 0, 0);
			SIE_write(SIE_SET_ADDRESS, (setup.wValue & 0x3F) | BIT7);
			break;
		case USB_GET_DESCRIPTOR:
			printstr("GET_DESCRIPTOR(");
			switch (setup.wValue >> 8) {
				case DESC_TYPE_DEVICE:
					printstr("device");
					send_device_descriptor(ep | BIT7);
					break;
				case DESC_TYPE_CONFIGURATION:
					printstr("configuration");
					write_endpoint(ep | BIT7, 0, 0);
					break;
				case DESC_TYPE_STRING:
					printstr("string");
					write_endpoint(ep | BIT7, 0, 0);
					break;
				case DESC_TYPE_INTERFACE:
					printstr("interface");
					write_endpoint(ep | BIT7, 0, 0);
					break;
				case DESC_TYPE_ENDPOINT:
					printstr("endpoint");
					write_endpoint(ep | BIT7, 0, 0);
					break;
				case DESC_TYPE_DEVICE_QUALIFIER:
					printstr("qualifier");
					write_endpoint(ep | BIT7, 0, 0);
					break;
				case DESC_TYPE_OTHER_SPEED_CONFIGURATION:
					printstr("configuration");
					write_endpoint(ep | BIT7, 0, 0);
					break;
				case DESC_TYPE_INTERFACE_POWER:
					printstr("power");
					write_endpoint(ep | BIT7, 0, 0);
					break;
				default:
						printhex(setup.wValue>>8);
						printhex(setup.wValue & 0xFF);
						write_endpoint(ep | BIT7, 0, 0);
						break;
			}
			printstr(")\r\n");
				
			
			break;
		case USB_SET_DESCRIPTOR:
			printstr("U SET_DESCIPTOR");
			break;
		case USB_GET_CONFIGURATION:
			printstr("U GET_CONFIGURATION");
// 			send_device_descriptor(ep | BIT7);
			break;
		case USB_SET_CONFIGURATION:
			printstr("U SET_CONFIGURATION");
			break;
		case USB_GET_INTERFACE:
			printstr("U GET_INTERFACE");
			break;
		case USB_SET_INTERFACE:
			printstr("U SET_INTERFACE");
			break;
		case USB_SYNCH_FRAME:
			printstr("U SYNCH_FRAME");
			break;
		default:
			printstr("UNKNOWN"); break;
			break;
	}
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

	// Slow interrupt
	if (dev_intr & EP_SLOW_INT) {

		for (unsigned char n=0; n<1; n++) {
			
			if (EP_INT_STAT & (1 << n)) {
				rxon();
				
				EP_INT_CLR = 1 << n;
				while ((DEV_INT_STAT & CDFULL_INT) == 0) ;
				uint32_t stat = CMD_DATA;

				printhex(n);
				printhex(stat);
				
				USB_CTRL = DEV_READ(n);

				while ((RX_PLENGTH & PKT_RDY) == 0 ) ;

				int len = (RX_PLENGTH & PLENGTH_MASK);
				printhex(len);

				if ((RX_PLENGTH & PKT_RDY) && (RX_PLENGTH & DV)) {
					uint32_t*  pBuf = packet_buffer;

					while (USB_CTRL & RD_EN) {
						*pBuf = RX_DATA;
						pBuf++;
					}

					USB_CTRL = 0;

					SIE_cmd(n);
					SIE_cmd(SIE_CLEAR_BUFFER);

					parse_packet(n, len, stat);
				}

				println();

			}
		}

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
	DEV_INT_EN = DEV_STAT_INT | EP_SLOW_INT | FRAME_INT;
	EP_INT_EN  = BIT4 | BIT3 | BIT2 | BIT0;

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
		for (j=0; j<200000; j++) ;
		FIO2CLR = BIT7;
		for (j=0; j<200000; j++) ;
		FIO2SET = BIT7;
	}
	
}
