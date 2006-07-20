
#include "core.h"
#include "list.h"
#include "interfaces.h"
#include <macros.h>

LIST_HEAD(inclitiometer_sensors);

void inclitiometer_register(struct inclitiometer* intf) {
	list_push_back(&inclitiometer_sensors, &intf->node);
}

struct inclitiometer* inclitiometer_acquire(uint32 inclitiometernum) {
	struct list_head* l;
	int num = 0;
	list_for_each(l,&inclitiometer_sensors) {
		if (num == inclitiometernum)
			return container_of(l,struct inclitiometer,node);
		num ++;
	}
	return NULL;
}
