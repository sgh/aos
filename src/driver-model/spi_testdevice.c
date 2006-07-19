#include "core.h"
#include "spi.h"
#include "deviceids.h"
#include "macros.h"
#include "spi_testdevice.h"
#include "interfaces.h"

extern struct spi_device_driver sca61t_device_driver;
extern struct inclination_sensor_interface sca61t_interface;

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
// 	inclination_sensor_register(&sca61t_interface);
}

static uint32 sca61t_inclination(struct device* dev) {
	printf("inclination called dev=%lu\n",(uint32)dev);
	return 666;
}

static uint8 sca61xx_ids[] = {6,5,4,DEV_SCA61T_ID,0};

// struct inclination_sensor_interface sca61t_interface = {
// 	.inclination = sca61t_inclination,
// 	.driver = &sca61t_device_driver.driver
// };

struct spi_device_driver sca61t_device_driver = {
	.id_table = sca61xx_ids,
	.driver = {
		.name = "SCAxxx inclitiometer driver",
		.bus = &spi_bus_type,
		.probe = sca61t_probe,
		.remove = sca61t_remove,
	},
};


