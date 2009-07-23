#ifndef LPC2364_USBDEV
#define LPC2364_USBDEV

// LPC23xx specific header

#define RD_EN   0x1
#define WR_EN   0x2
#define DEV_READ(ep)  (RD_EN | (((ep) & 0x0F) << 2))
#define DEV_WRITE(ep) (WR_EN | (((ep) & 0x0F) << 2))

#define PLENGTH_MASK 0x3FF

#define PKT_RDY BIT11
#define DV      BIT10

// Interrupt bits
#define FRAME_INT    BIT0
#define EP_FAST_INT  BIT1
#define EP_SLOW_INT  BIT2
#define DEV_STAT_INT BIT3
#define CCEMPTY_INT  BIT4
#define CDFULL_INT   BIT5
#define RX_ENDPKT    BIT6
#define TX_ENDPKT    BIT7
#define EP_RLZED_INT BIT8
#define ERR_INT      BIT9


// SIE bits
#define SIE_WRITE_PHASE    0x100
#define SIE_READ_PHASE     0x200
#define SIE_COMMAND_PHASE  0x500


// SIE Commands
#define SIE_SET_ADDRESS        0xD0
#define SIE_SET_DEVICE_STATUS  0xFE
#define SIE_VALIDATE_BUFFER    0xFA
#define SIE_CLEAR_BUFFER       0xF2
#define SIE_CONFIGURE_DEVICE   0xD8

#endif