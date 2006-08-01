#ifndef INCLITIOMETER_H
#define INCLITIOMETER_H

#include <core.h>
#include <driver_class.h>

struct inclitiometer_class_fops {
	int32 (*inclination)(struct device* dev, uint32* inclination);
};

struct inclitiometer {
	struct inclitiometer_class_fops* fops;
	struct device* dev;
};

extern struct driver_class inclitiometer_class;


static inline int acquire_inclitiometer(int idx, struct inclitiometer* inclitiometer) {
	return acquire_driver_class_fops(&inclitiometer_class, &inclitiometer->dev, (void**)&inclitiometer->fops, idx);
}

static inline int get_inclination(struct inclitiometer* incl, uint32* inclination) {
	return incl->fops->inclination(incl->dev,inclination);
}


#endif
