#ifndef SPI_TESTDEVICE_H
#define SPI_TESTDEVICE_H

#include <types.h>
#include "core.h"

struct inclitiometer_interface {
	uint32 (*inclination)(struct device*);
};

#endif

