#include <stdint.h>
#include "usbcore.h"

uint8_t rxled;
uint8_t txled;
uint8_t dataled;

// Generic packet-buffer for use in endpoint callback functions
static uint32_t _packet_buffer[16]; // 64 bytes

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


static const struct configuration_descriptor* _configuration_descriptor;
static const struct device_descriptor* _device_descriptor;


static void _send_device_descriptor(uint8_t ep) {
	if (_device_descriptor)
		usbdev_write_endpoint(ep, (uint8_t*)_device_descriptor, _device_descriptor->bLength);
	else
		usbdev_write_endpoint(ep, 0, 0);
}

static void _send_configuration_descriptor(uint8_t ep) {
	if (_configuration_descriptor)
	usbdev_write_endpoint(ep, (uint8_t*)_configuration_descriptor, _configuration_descriptor->wTotalLength);
	else
		usbdev_write_endpoint(ep, 0, 0);
}

void parse_control_packet(uint8_t pEp, uint32_t stat) {
	uint8_t lep = pEp >> 1;
	struct setup_packet setup;
	
	if (! (stat & EP_STAT_STP))
		return;
	
	int len = usbdev_read_endpoint(pEp, &setup, sizeof(setup) );

	switch (setup.bRequest) {
		case USB_GET_STATUS:
			printstr("U GET_STATUS");
			usbdev_write_endpoint(lep | BIT7, (uint8_t*)"\0", 1);
			break;
		case USB_CLEAR_FEATURE:
			printstr("U CLEAR_FEATURE");
			usbdev_write_endpoint(lep | BIT7, 0, 0);
			break;
		case USB_SET_FEATURE:
			printstr("U SET_FEATURE");
			usbdev_write_endpoint(lep | BIT7, 0, 0);
			break;
		case USB_SET_ADDRESS:
			printstr("SET_ADDRESS ");
			printhex(setup.wValue);
			usbdev_write_endpoint(lep | BIT7, 0, 0);
			usbdev_set_address(setup.wValue & 0x7F);
			break;
		case USB_GET_DESCRIPTOR:
			printstr("GET_DESCRIPTOR(");
			switch (setup.wValue >> 8) {
				case DESC_TYPE_DEVICE:
					printstr("device");
					_send_device_descriptor(lep | BIT7);
					break;
				case DESC_TYPE_CONFIGURATION:
					printstr("configuration");
					_send_configuration_descriptor(lep | BIT7);
					break;
				case DESC_TYPE_STRING:
					printstr("U string");
					usbdev_write_endpoint(lep | BIT7, 0, 0);
					break;
				case DESC_TYPE_INTERFACE:
					printstr("U interface");
					usbdev_write_endpoint(lep | BIT7, 0, 0);
					break;
				case DESC_TYPE_ENDPOINT:
					printstr("U endpoint");
					usbdev_write_endpoint(lep | BIT7, 0, 0);
					break;
				case DESC_TYPE_DEVICE_QUALIFIER:
					printstr("U qualifier");
					usbdev_write_endpoint(lep | BIT7, 0, 0);
					break;
				case DESC_TYPE_OTHER_SPEED_CONFIGURATION:
					printstr("U configuration");
					usbdev_write_endpoint(lep | BIT7, 0, 0);
					break;
				case DESC_TYPE_INTERFACE_POWER:
					printstr("U power");
					usbdev_write_endpoint(lep | BIT7, 0, 0);
					break;
				default:
						printhex(setup.wValue>>8);
						printhex(setup.wValue & 0xFF);
						usbdev_write_endpoint(lep | BIT7, 0, 0);
						break;
			}
			printstr(")");
				
			
			break;
		case USB_SET_DESCRIPTOR:
			printstr("U SET_DESCRIPTOR ");
			printhex(setup.wValue);
			break;
		case USB_GET_CONFIGURATION:
			printstr("GET_CONFIGURATION");
			_send_configuration_descriptor(lep | BIT7);
			break;
		case USB_SET_CONFIGURATION:
			printstr("SET_CONFIGURATION ");
			printhex(setup.wValue);
			
			// TODO: investigate _configuration_descriptor on how to configura the device
			usbdev_realize_endpoint(4, 64);
			usbdev_realize_endpoint(5, 64);
			usbdev_set_configured(1);
			
			usbdev_write_endpoint(lep | BIT7, 0, 0);
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
	println();
}

void endpoint_input(uint8_t pEp, uint32_t stat) {
	uint8_t lEp = pEp >> 1;
	int len = usbdev_read_endpoint(pEp, _packet_buffer, sizeof(_packet_buffer));
// 	static int count = 0;
// 	count++;
// 	usbdev_write_endpoint(ep | BIT7, 0, 8);
	dataon();
	usbdev_write_endpoint( lEp | BIT7, _packet_buffer, len);
}

void usbcore_init(struct configuration_descriptor* conf, struct device_descriptor* dev) {
	_configuration_descriptor = conf;
	_device_descriptor = dev;
}

void usbcore_device_endpoint_in(uint8_t pEp, uint32_t stat) {
	if ((pEp>>1) == 0) {
		parse_control_packet(pEp, stat);
	} else
		endpoint_input(pEp, stat);
}
