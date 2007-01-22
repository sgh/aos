/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#define AOS_KERNEL_MODULE

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



// // int bus_for_each_dev(struct bus_type * bus, struct device * start, void * data, int (*fn)(struct device *, void *)) {
// 	return 0;
// }

// int bus_for_each_drv(struct bus_type * bus, struct device_driver * start, void * data, int (*fn)(struct device_driver *, void *)) {
// 	struct list_head* drv;
// 	list_for_each_from(drv, &bus->drivers, &start->bus_driver_list) {
// 		printf("driver: %s\n", container_of(drv, struct device_driver, bus_driver_list)->name);
// 	}
// 	return 0;
// }
