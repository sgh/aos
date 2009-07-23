#include "usbcore.h"
#include "usbdev_lpc23xx.h"
#include <arm/lpc23xx.h>
#include <aos/irq.h>


/** convert from endpoint address to endpoint index */
#define EP2IDX(bEP) (( ((bEP)&0xF) <<1) | (((bEP)&0x80)>>7 ))
/** convert from endpoint index to endpoint address */
#define IDX2EP(idx) ((((idx)<<7)&0x80)|(((idx)>>1)&0xF))

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


void usbdev_write_endpoint(uint8_t ep, uint8_t* data, int len) {
	txon();
	
	USB_CTRL   = DEV_WRITE(ep);
	
	TX_PLENGTH = len;

	while (USB_CTRL & WR_EN) {
		TX_DATA = (data[3]<<24) | (data[2]<<16) | (data[1]<<8) | (data[0]);
		data += 4;
	}

	USB_CTRL = 0;

	SIE_cmd(EP2IDX(ep));
	SIE_cmd(SIE_VALIDATE_BUFFER);

}


int usbdev_read_endpoint(int pEp, uint32_t* buf, int buflen) {
		// Set ready bit
	USB_CTRL = DEV_READ(pEp >> 1);

	// Wait until package is ready for readout
	while ((RX_PLENGTH & PKT_RDY) == 0 ) ;

	int len = (RX_PLENGTH & PLENGTH_MASK);

	// return -1 if data is not valid or packet does not fit in the buffer
	if ( ((RX_PLENGTH & DV) == 0) || (buflen < len)) {
		len = -1;
		goto out;
	}
	
	if (likely(buf!=NULL)) {
		while (USB_CTRL & RD_EN) {
			*buf = RX_DATA;
			buf++;
		}
	}

out:
	USB_CTRL = 0;

	SIE_cmd(pEp);
	SIE_cmd(SIE_CLEAR_BUFFER);
	
	return len;
}

void usbdev_set_configured(int configured) {
		SIE_write(SIE_CONFIGURE_DEVICE, configured ? 1 : 0);
}

void usbdev_realize_endpoint(uint8_t pEp, int maxpkgsize){
	DEV_INT_CLR = EP_RLZED_INT;
	
	REALIZE_EP |= 1<<pEp;
	EP_INDEX = pEp;
	MAXPACKET_SIZE = maxpkgsize;
	while ((DEV_INT_STAT & EP_RLZED_INT) == 0) ;
	DEV_INT_CLR = EP_RLZED_INT;
}

void usbdev_set_address(uint32_t addr) {
	SIE_write(SIE_SET_ADDRESS, addr | BIT7);
}

void usbdev_reset(void) {
	SIE_write(SIE_SET_DEVICE_STATUS, 0x10);
}

void usbdev_interrupt_handler(UNUSED void* arg) {
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

				usbcore_device_endpoint_in(pEp, stat);
			}

		}
		USB_CTRL = 0;
	}

	DEV_INT_CLR = dev_intr;

}

void usbdev_init() {
		// Power up the USB-controller
	PCONP |= BIT31;

	// Initialize USB clocks
	OTG_CLK_CTRL |= 0x12;
	while ((OTG_CLK_STAT & 0x12) != 0x12) ;

	// Enable USB pins
	PINSEL1 |= (0x01 << 26); // D+
	PINSEL1 |= (0x01 << 28); // D-
	PINSEL3 |= (0x10 << 28); // Vbus
	PINSEL3 |= (0x01 << 2);  // GoodLink
// 	FIO2DIR |= BIT9;
// 	FIO2CLR = BIT9;

	// Disable pull-ups on Vbus
	PINMODE3 &= ~(0x10 << 28);
	
	// Enable USB interrupts
	DEV_INT_EN = DEV_STAT_INT | EP_FAST_INT | EP_SLOW_INT | FRAME_INT;
	EP_INT_EN  = BIT5 | BIT4 | BIT2 | BIT0;
}