
#include "core.h"
#include "list.h"
#include "interfaces.h"

LIST_HEAD(inclitiometer_sensors);

void inclination_sensor_register(struct inclination_sensor_interface* intf) {
	list_push_back(&inclitiometer_sensors, &intf->node);
}