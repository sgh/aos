#include "core.h"

#include "spi.h"
#include <list.h>
#include <macros.h>

struct bus_type spi_bus_type;

struct spi_master master;

void init_spi() {
	bus_register(&spi_bus_type);
}

void spi_driver_register(struct spi_device_driver* drv) {
	drv->driver.bus = &spi_bus_type;
	list_push_back(&spi_bus_type.drivers, &drv->driver.bus_driver_list);
	driver_register(&drv->driver);
}

struct spi_device_driver* to_spi_driver(struct device_driver* drv) {
	return container_of( drv , struct spi_device_driver, driver);
}

static int spi_match(struct device* dev, struct device_driver* drv) {
	struct spi_device_driver *spidrv = to_spi_driver(drv);
	const uint8* id = spidrv->id_table;
// 	printf("match dev(%s) drv(%s)\n",dev->name, drv->name);
	while (id && *id != 0) {
// 		printf("%d\n",*id);
		if (dev->device_id == *id && drv->probe(dev)) {
			printf("%s will be bound to %s\n",dev->name,drv->name);
			return 1;
		}
		id++;
	}
	return 0;
}

void spi_device_register(struct spi_device* spidev) {
	struct device *dev = &spidev->dev;
	dev->bus = &spi_bus_type;
	spidev->master = &master;
	device_register(dev);
}

static void spi_test(struct device* dev) {
	printf("spi test\n");
}

struct bus_type spi_bus_type = {
	.name = "spi",
	.suspend = 0,
	.resume = 0,
	.match = spi_match,
};


struct spi_master master = {
	.test = spi_test
};
