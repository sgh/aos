#ifndef INCLITIOMETER_H
#define INCLITIOMETER_H

#include "core.h"
#include <types.h>
#include <list.h>

extern struct list_head inclitiometer_sensors;

/** TRY THIS */

struct inclitiometer_fops {
	int32 (*get_inclination)(struct device* dev, uint32* inclination);
};

struct inclitiometer {
	struct inclitiometer_fops* fops;
	struct device* dev;
	struct list_head node;					/**< \brief Node in interface-instance list */
};

static inline uint32 get_inclination(struct inclitiometer* inclmeter, uint32* inclination) {
	return inclmeter->fops->get_inclination(inclmeter->dev, inclination);
}
/** TRY THIS */


void inclitiometer_register(struct inclitiometer* intf);

struct inclitiometer* inclitiometer_acquire(uint32 inclitiometernum);

#endif
