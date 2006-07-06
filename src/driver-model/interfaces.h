
#include "core.h"
#include <types.h>
#include <list.h>

extern struct list_head inclitiometer_sensors;



struct inclination_sensor_interface {
	uint32 (*inclination)(struct device* dev);
		
	struct device_driver *driver;
	struct list_head node;					/**< \brief Node in interface-instance list */
};


void inclination_sensor_register(struct inclination_sensor_interface* intf);
