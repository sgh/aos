
#include <inclitiometer.h>

struct driver_class inclitiometer_class = {
.name = "Inclitiometers"
};

static void _init_ inclitiometer_class_init() {
	class_register(&inclitiometer_class);
}
