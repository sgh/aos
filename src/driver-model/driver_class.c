
#include <driver_core.h>
#include "list.h"
#include "driver_class.h"
#include "errno.h"
#include <macros.h>

int class_fops_register(struct driver_class* cls, struct driver_class_fops* fops) {
	printf("%s driver fops registered from device driver \"%s\"\n",cls->name, fops->driver->name);
	list_push_back(&cls->class_fops, &fops->node);
	return 0;
}

int acquire_driver_class_fops(struct driver_class* cls, struct device** dev, void** fops, int devicenum) {
	int num = 0;
	char selected = 0;
	struct list_head* drivers;
	struct list_head* devices;

	*dev = 0;
	*fops = 0;

	list_for_each(drivers, &cls->class_fops) {
		struct driver_class_fops* class_fops = container_of(drivers, struct driver_class_fops, node);
		struct device_driver* drv = class_fops->driver;
		printf("%s\n", drv->name);
		*fops = class_fops->fops;

		list_for_each(devices, &drv->devices) {
			struct device* _dev = container_of(devices, struct device, driver_list);

			printf("   %s%s\n", _dev->name, num==devicenum?" (selected)":"");
			if (dev)
				*dev = _dev;

			if (num==devicenum) {
				selected = 1;
				break;
			}

			num++;
		}
		
		if (selected)
			break;
	}
	
	if (!selected)
		return ENODEV;
	
	return 0;
}

