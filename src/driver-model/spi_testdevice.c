#include "core.h"
#include "spi.h"
#include "deviceids.h"
#include "macros.h"

extern struct spi_device_driver sca61t_device_driver;

static int sca61t_probe(struct device* dev) {
	struct spi_device *spi = container_of( dev, struct spi_device, dev);
	if (spi->master)
		spi->master->test(dev);
	printf("sca61t ptobe\n");
	return 1;
}

static int sca61t_remove(struct device* dev) {
	return 1;
}


void sca61t_init() {
	spi_driver_register(&sca61t_device_driver);
}

static uint8 sca61xx_ids[] = {6,5,4,DEV_SCA61T_ID,0};


struct spi_device_driver sca61t_device_driver = {
	.id_table = sca61xx_ids,
	.driver = {
		.name = "SCAxxx inclitiometer driver",
		.bus = &spi_bus_type,
		.probe = sca61t_probe,
		.remove = sca61t_remove,
	},
};
