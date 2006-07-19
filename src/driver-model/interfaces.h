
#include "core.h"
#include <types.h>
#include <list.h>

extern struct list_head inclitiometer_sensors;



struct inclination_sensor_interface {
// 	uint32 (*inclination)(struct device* dev);
		
// 	struct device_driver *driver;
	struct list_head node;					/**< \brief Node in interface-instance list */
};

/** TRY THIS */
struct can_controller {
	struct file_operations* fops;
	struct device* dev;
};

static inline uint32 write_can(struct can_controller* controller, struct can_message* msg) {
	controller->fops->write(controller->dev, msg);
}
/** TRY THIS */


void inclination_sensor_register(struct inclination_sensor_interface* intf);
