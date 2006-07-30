
#include <inclitiometer.h>

static struct driver_class inclitiometer_class = {
.name = "Inclitiometers"
};

void inclitiometer_class_init() {
	class_register(&inclitiometer_class);
}
