
#include <serio.h>

struct driver_class serio_class = {
.name = "Serial IO",
};

void  serio_class_init() {
	class_register(&serio_class);
}

CLASS_MODULE_INIT(serio_class_init);
