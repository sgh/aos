#include <usb.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	struct usb_bus *busses;
	struct usb_device* mydev = NULL;
	usb_dev_handle* myhandle = NULL;

	usb_set_debug(3);
	
	usb_init();
	usb_find_busses();
	usb_find_devices();

	busses = usb_get_busses();

  struct usb_bus *bus;
	int c, i, a;

	/* ... */

	for (bus = busses; bus; bus = bus->next) {
		struct usb_device *dev;

		for (dev = bus->devices; dev; dev = dev->next) {
			/* Check if this device is a printer */
			if (dev->descriptor.idVendor == 0xFFFF) {
				mydev = dev;
				break;
				/* Open the device, claim the interface and do your processing */
			}

// 			/* Loop through all of the configurations */
// 			for (c = 0; c < dev->descriptor.bNumConfigurations; c++) {
// 				/* Loop through all of the interfaces */
// 				for (i = 0; i < dev->config[c].bNumInterfaces; i++) {
// 					/* Loop through all of the alternate settings */
// 					for (a = 0; a < dev->config[c].interface[i].num_altsetting; a++) {
// 						/* Check if this interface is a printer */
// 						if (dev->config[c].interface[i].altsetting[a].bInterfaceClass == 7) {
// 							/* Open the device, set the alternate setting, claim the interface and do your processing */
// 
// 						}
// 					}
// 				}
// 			}
		}
	}
	char buf[1024];
	int count = 0;
	if (mydev) {
		int res;
		printf("device found\n");
		myhandle = usb_open(mydev);
		res = usb_claim_interface(myhandle, 0);
		printf("res=%d\n", res);
// TODO : Apparently this actually works, but nothing is received on the device
		while (1) {
			res = usb_bulk_write(myhandle, 0x2, buf, 2048, 500);
	// 		res = usb_control_msg(myhandle, 0, 0, 0, 0, 0, 0, 500);
			if (res == 2048) {
				printf("%d\n",count);
// 				fflush(0);
			} else
				printf("res=%d\n", res);
// 			usleep(1000);
			count++;
		}
	}

	
}