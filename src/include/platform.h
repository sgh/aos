#ifndef SPI_H
#define SPI_H

#include <types.h>
#include <driver_core.h>

extern struct bus_type platform_bus_type;

struct platform_device_driver {
	const uint8_t *id_table;
	struct device_driver driver;
};

struct platform_device {
	struct device dev;
};

void platform_driver_register(struct platform_device_driver* drv);
void platform_device_register(struct platform_device* dev);

#endif
