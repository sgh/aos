
#include <stdio.h>
#include <list.h>
#include <macros.h>

#include "core.h"
#include "gpio.h"
#include "spi.h"
#include "deviceids.h"
#include "interfaces.h"

#define SJA1000_DEVID 1
#define TLE2903_DEVID 2

void do_initcalls() {
	// Init busses
	init_spi();
	
	// Init drivers
	sca61t_init();
	
}



struct spi_device inclitiometer = {
	.dev = {
		.name = "SCA61T inclitiometer",
		.device_id = DEV_SCA61T_ID,
	}
};


struct gpio testio = {
	.port = 3,
	.range = 2,
	.shifts = 1,
};

void func(int n) {
	printf("call %d\n",n);
}



struct structfunc {
	int dev;
	void (*call)(int n);
};


struct structfunc f = {
	.call = func
};

	

int main() {
	struct list_head* l;
	struct list_head* d;
	struct inclitiometer* incl;
	do_initcalls();

	spi_device_register(&inclitiometer);
	
	/*gpio_set(&testio);
	gpio_clear(&testio);
	gpio_get(&testio,&dat);
	gpio_highimpedance(&testio);
	gpio_lowimpedance(&testio);
	gpio_data(&testio,2);*/
	
	printf("Devices:\n");
	list_for_each(l,&global_device_list) {
		struct device_driver *driver = container_of(l,struct device,g_list)->driver;
		printf("   %s owned by \"%s\"\n", container_of(l,struct device,g_list)->name, driver?driver->name:"none");
	}
	
	
	
	printf("Busses:\n");
	list_for_each(l,&global_bus_list) {
		struct list_head *drivers = &container_of(l,struct bus_type,g_list)->drivers;
		printf("   %s (",container_of(l,struct bus_type,g_list)->name);
		list_for_each(d, drivers)
			printf("\"%s\" ",container_of(d,struct device_driver,bus_driver_list)->name);

		printf(")\n");
	}
	
	
	printf("Drivers:\n");
	list_for_each(l, &global_driver_list) {
		struct device_driver* drv = container_of(l,struct device_driver,g_list);
		printf("   %s (",drv->name);
		list_for_each(d, &drv->devices)
			printf("\"%s\" ",container_of(d,struct device,driver_list)->name);
		printf(")\n");
	}
	
	
	printf("Inclination devices:\n");
	list_for_each(l, &inclitiometer_sensors) {
		struct device_driver* drv = container_of(l,struct inclitiometer, node)->dev->driver;
		printf("   %s (",drv->name);
		list_for_each(d, &drv->devices) {
			printf(" %s ",container_of(d, struct device, driver_list)->name);
		}
		printf(")");
	}
	
	uint32 val;
	
	if ((incl = inclitiometer_acquire(0)) == NULL)
		printf("failed\n");
	get_inclination(incl,&val);
	printf("return %d\n",val);
	
	
// 	if (container_of(l,struct inclination_sensor_interface, node)->driver == 
// 		container_of(l,struct inclination_sensor_interface, node)->inclination(0);
	
	
	
	printf("\n");
// 	bus_for_each_drv(&spi_bus_type,  container_of(spi_bus_type.drivers.next, struct device_driver, bus_driver_list) , 0, 0);
	
	return 0;
}
