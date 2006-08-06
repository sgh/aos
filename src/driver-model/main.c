

#include <stdio.h>
#include <spi.h>
#include <inclitiometer.h>
#include <driver_core.h>

#define SJA1000_DEVID 1
#define TLE2903_DEVID 2

void do_initcalls() {
	// Init classes
// 	inclitiometer_class_init();

	// Init busses
// 	init_spi();
	
	// Init drivers
	sca61t_init();
}

struct spi_device inclitiometer1 = {
	.dev = {
		.name = "SCA61T inclitiometer 0",
		.device_id = DEV_SCA61T_ID,
	}
};

struct spi_device inclitiometer2 = {
	.dev = {
		.name = "SCA61T inclitiometer 1",
		.device_id = DEV_SCA61T_ID,
	}
};


struct gpio testio = {
	.port = 3,
	.range = 2,
	.shifts = 1,
};


int main() {
	struct list_head* l;
	struct list_head* d;
	uint32 inclination;
	struct inclitiometer incl;
	do_initcalls();
	

	spi_device_register(&inclitiometer1);
	spi_device_register(&inclitiometer2);
	
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
	
	
	printf("Classes:\n");
	list_for_each(l,&global_class_list) {
		struct driver_class *cls = container_of(l,struct driver_class,g_list);
		printf("   %s ", cls->name);
	}
	printf("\n");
	
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
	
	printf("Testing inclitiometer api\n");
	
	if (acquire_inclitiometer(0, &incl) == 0) {
		if (get_inclination(&incl,&inclination))
			printf("inclination failed\n");
	} else
		printf("No such inclitiometer\n");
	
// 	if (container_of(l,struct inclination_sensor_interface, node)->driver == 
// 		container_of(l,struct inclination_sensor_interface, node)->inclination(0);
	
	


	device_ioctl(incl.dev,5);
	
	printf("\n");
// 	bus_for_each_drv(&spi_bus_type,  container_of(spi_bus_type.drivers.next, struct device_driver, bus_driver_list) , 0, 0);
	
	return 0;
}
