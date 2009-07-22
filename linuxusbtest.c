#include <usb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	char buf1[128];
	char buf2[128];
	
	memset(buf1, 0, sizeof(buf1));
	memset(buf2, 0, sizeof(buf2));
	int count = 0;
	if (mydev) {
		int res;
		printf("device found\n");
		myhandle = usb_open(mydev);
		res = usb_claim_interface(myhandle, 0);
		printf("res=%d\n", res);

		// TODO Stuff could get stuf in receive-buffer so reset device or clear buffers or something
		
		while (1) {
			int size;
			size = sprintf(buf1, "T %d", count) + 1;
			
			res = usb_bulk_write(myhandle, 2, buf1, size, 1000);
			if (res != size)
				printf("Write: res=%d\n", res);

			memset(buf2, 0, sizeof(buf2));
			res = usb_bulk_read(myhandle, 2, buf2, sizeof(buf2), 1000);
			
			if (strcmp(buf1, buf2) != 0) {
				printf("%8s : %8s\n", buf1, buf2);
				printf("Read: res=%d\n", res);
			} 
// 			else {
// 				printf(".");
// 				fflush(0);
// 			}
			count++;
// 			usleep(10000);
		}
	}

	
}