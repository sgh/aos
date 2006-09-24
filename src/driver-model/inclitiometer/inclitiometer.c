
#include <inclitiometer.h>

struct driver_class inclitiometer_class = {
.name = "Inclitiometers",
};

static void  inclitiometer_class_init() {
	class_register(&inclitiometer_class);
}

DRIVER_MODULE_INIT(inclitiometer_class_init);
