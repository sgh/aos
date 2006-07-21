#include "core.h"
#include "spi.h"
#include "deviceids.h"
#include "macros.h"
#include "spi_testdevice.h"
#include "interfaces.h"

extern struct spi_device_driver sca61t_device_driver;
extern struct inclitiometer_fops sca61t_inclitiometer_fops;

static int sca61t_probe(struct device* dev) {
	struct spi_device *spi = container_of( dev, struct spi_device, dev);
	struct inclitiometer* inclitiometer;
	if (spi->master)
		spi->master->test(dev);
	printf("sca61t probe\n");
	
	inclitiometer = malloc(sizeof(struct inclitiometer));
	memset(inclitiometer,0,sizeof(struct inclitiometer));
	inclitiometer->dev = dev;
	inclitiometer->fops = &sca61t_inclitiometer_fops;
	inclitiometer_register(inclitiometer);
	return 1;
}

static int sca61t_remove(struct device* dev) {
	return 1;
}


void sca61t_init() {
	spi_driver_register(&sca61t_device_driver);
}

static int32 sca61t_inclination(struct device* dev, uint32* inclination) {
	printf("inclination called (%s)\n",dev->name);
	*inclination = 666;
	return 0;
}

static uint8 sca61xx_ids[] = {6,5,4,DEV_SCA61T_ID,0};

struct inclitiometer_fops sca61t_inclitiometer_fops = {
	.get_inclination = sca61t_inclination
};

struct spi_device_driver sca61t_device_driver = {
	.id_table = sca61xx_ids,
	.driver = {
		.name = "SCAxxx inclitiometer driver",
		.bus = &spi_bus_type,
		.probe = sca61t_probe,
		.remove = sca61t_remove,
	},
};


