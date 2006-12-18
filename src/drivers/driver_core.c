#include <driver_core.h>
#include "spi.h"
#include <list.h>
#include <macros.h>

LIST_HEAD(global_device_list);

LIST_HEAD(global_bus_list);

LIST_HEAD(global_driver_list);

LIST_HEAD(global_class_list);

void device_register(struct device* dev) {
	struct list_head* it;
	struct device_driver* drv;

	list_for_each(it, &dev->bus->drivers) {
		drv = container_of(it,struct device_driver,bus_driver_list);
			
		if (dev->bus && dev->bus->match) {
			if (dev->bus->match(dev,drv)) {
				dev->driver = drv;
				list_push_back(&drv->devices, &dev->driver_list);
			}
		}
	}

	list_push_back(&global_device_list, &dev->g_list);
}


void bus_register(struct bus_type* bus) {
	INIT_LIST_HEAD(&bus->drivers);
	list_push_back(&global_bus_list, &bus->g_list);
}

void driver_register(struct device_driver* drv) {
	INIT_LIST_HEAD(&drv->devices);
	list_push_back(&global_driver_list, &drv->g_list);
}

void class_register(struct driver_class* cls) {
// 	printf("Class register: %s\n",cls->name);
	INIT_LIST_HEAD(&cls->class_fops);
	list_push_back(&global_class_list,&cls->g_list);
}



int bus_for_each_dev(struct bus_type * bus, struct device * start, void * data, int (*fn)(struct device *, void *)) {
	return 0;
}

int bus_for_each_drv(struct bus_type * bus, struct device_driver * start, void * data, int (*fn)(struct device_driver *, void *)) {
	struct list_head* drv;
	list_for_each_from(drv, &bus->drivers, &start->bus_driver_list) {
// 		printf("driver: %s\n", container_of(drv, struct device_driver, bus_driver_list)->name);
	}
	return 0;
}
