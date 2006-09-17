#ifndef SPI_H
#define SPI_H

#include <types.h>
#include <driver_core.h>

extern struct bus_type spi_bus_type;

struct spi_device_driver {
	const uint8_t *id_table;
	struct device_driver driver;
};

struct spi_master {
	void (*test)(struct device *dev);
};

struct spi_device {
	struct spi_master* master;
	struct device dev;
};

void spi_driver_register(struct spi_device_driver* drv);
void spi_device_register(struct spi_device* dev);

#endif
