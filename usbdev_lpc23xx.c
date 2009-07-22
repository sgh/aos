#include "usbcore.h"
#include "usbdev_lpc23xx.h"


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


void write_endpoint(uint8_t ep, uint8_t* data, int len) {
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


int read_endpoint(int pEp, uint32_t* buf, int buflen) {
		// Set ready bit
	USB_CTRL = DEV_READ(pEp >> 1);

	// Wait until package is ready for readout
	while ((RX_PLENGTH & PKT_RDY) == 0 ) ;

	int len = (RX_PLENGTH & PLENGTH_MASK);

	// If data is valid read the package
	if ((RX_PLENGTH & DV) == 0) {
		len = -1;
		goto out;
	}
	
	if (likely(buf)) {
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