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
#include "platform.h"
#include <list.h>
#include <macros.h>

struct bus_type platform_bus_type;


void init_platform() {
	bus_register(&platform_bus_type);
}

void platform_driver_register(struct platform_device_driver* drv) {
	drv->driver.bus = &platform_bus_type;
	list_push_back(&platform_bus_type.drivers, &drv->driver.bus_driver_list);
	driver_register(&drv->driver);
}

void platform_device_register(struct platform_device* platformdev) {
	struct device *dev = &platformdev->dev;
	dev->bus = &platform_bus_type;
	device_register(dev);
}

struct platform_device_driver* to_platform_driver(struct device_driver* drv) {
	return container_of( drv , struct platform_device_driver, driver);
}


static int platform_match(struct device* dev, struct device_driver* drv) {
	struct platform_device_driver *platformdrv = to_platform_driver(drv);
	const uint8_t* id = platformdrv->id_table;

// 	printf("match dev(%s) drv(%s)\n",dev->name, drv->name);
	while (id && *id != 0) {
// 		printf("%d\n",*id);
		if (dev->device_id == *id && drv->probe(dev)) {
// 			printf("%s will be bound to %s\n",dev->name,drv->name);
			return 1;
		}
		id++;
	}
	return 0;
}

struct bus_type platform_bus_type = {
	.name = "platform",
	.suspend = 0,
	.resume = 0,
	.match = platform_match,
};

BUS_MODULE_INIT(init_platform);
