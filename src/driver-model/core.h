#ifndef CORE_H
#define CORE_H

#include "gpio.h"
#include <queue.h>

#define DEVICE_NAME_SIZE 32
#define BUS_ID_SIZE 32

extern struct list_head global_device_list;

extern struct list_head global_bus_list;

extern struct list_head global_driver_list;

struct bus_type;
struct device;
struct device_driver;

struct device {
	struct list_head g_list;			// Node in the global device-list
	struct list_head bus_list;		// Node in the bus' device-list
	struct list_head driver_list;	// Node in the drivers' device-list
	
	uint8 device_id;							// Device-ID. Since PnP is not available, ID's must be hardcoded.
	
	struct device_driver* driver;	// The driver that owns this device.
	
	char name[DEVICE_NAME_SIZE];	// Name of the device
	char bus_id[BUS_ID_SIZE];			// ID on the bus. Unique for every bus
	
 	struct bus_type* bus;					// The bus this device is on
	void* driver_data;						// Driver-specific data. Drivers must use this for whatever per device stats.
	void* platform_data;					// Platform specific-data. This will typicati be a pointer to a struct gpio
	
	uint8 current_state;					// The current state of the device.
	
};

struct bus_type {
	struct list_head g_list;		// Node in global devicelist
	struct list_head drivers;		// List of owned drivers
	char name[BUS_ID_SIZE];
	int (*suspend)(struct device *dev);
	int (*resume)(struct device *dev);
	int (*match)(struct device *dev, struct device_driver *drv);
};

struct device_driver {
	char* name;
	struct list_head g_list;						// Node in global driver list
	struct list_head bus_driver_list;		// Node in bus' driver list
	struct list_head devices;						// List of owned devices
	
	struct bus_type* bus;
	int (*probe)(struct device * dev);
	int (*remove)(struct device * dev);
	int (*suspend)(struct device * dev);
	int (*resume)(struct device * dev);
};

void bus_register(struct bus_type* bus);

void device_register(struct device* dev);

void driver_register(struct device_driver* drv);

int bus_for_each_dev(struct bus_type * bus, struct device * start, void * data, int (*fn)(struct device *, void *));

int bus_for_each_drv(struct bus_type * bus, struct device_driver * start, void * data, int (*fn)(struct device_driver *, void *));


#endif
