#include <driver_core.h>
#include <driver_class.h>
#include <inclitiometer.h>
#include "spi.h"
#include "deviceids.h"
#include "macros.h"
#include "sca61t.h"
#include "errno.h"

extern struct spi_device_driver sca61t_device_driver;
extern struct driver_class_fops sca61t_inclination_driver_fops;

static int sca61t_probe(struct device* dev) {
	struct spi_device *spi = container_of( dev, struct spi_device, dev);
	if (spi->master)
		spi->master->test(dev);
	printf("sca61t probe\n");

	return 1;
}

static int sca61t_remove(struct device* dev) {
	return 1;
}


void sca61t_init() {
	printf("sca61t init\n");
	spi_driver_register(&sca61t_device_driver);
	class_fops_register(&inclitiometer_class, &sca61t_inclination_driver_fops);
}

static int32 sca61t_inclination(struct device* dev, uint32* inclination) {
	printf("inclination called (%s)\n",dev->name);
	*inclination = 666;
	return 0;
}

static uint8 sca61xx_ids[] = {6,5,4,DEV_SCA61T_ID,0};



struct inclitiometer_class_fops sca61t_inclination_fops = {
	.inclination = sca61t_inclination
};

struct driver_class_fops sca61t_inclination_driver_fops = {
	.fops = &sca61t_inclination_fops,
	.driver = &sca61t_device_driver.driver
};

static int sca61t_ioctl(struct device* dev, int request, ...) {
	printf("IOCTL %d\n",request);
	return 0;
}


struct spi_device_driver sca61t_device_driver = {
	.id_table = sca61xx_ids,
	.driver = {
		.name = "SCAxxx inclitiometer driver",
		.bus = &spi_bus_type,
		.probe = sca61t_probe,
		.remove = sca61t_remove,
		.ioctl = sca61t_ioctl,
	},
};


