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
