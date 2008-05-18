/*
		AOS - ARM Operating System
		Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

		This library is free software; you can redistribute it and/or
		modify it under the terms of the GNU Lesser General Public
		License as published by the Free Software Foundation; either
		version 2.1 of the License, or (at your option) any later version.

		This library is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
		Lesser General Public License for more details.

		You should have received a copy of the GNU Lesser General Public
		License along with this library; if not, write to the Free Software
		Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
		*/
#ifndef INCLITIOMETER_H
#define INCLITIOMETER_H

#include <aos_module.h>

#include <driver_core.h>
#include <driver_class.h>

/**
 * \brief Function-pointers for all inclitiometers
 */
struct inclitiometer_class_fops {
	/**
	 * \brief Get inclination
	 * @param dev The ::device in question.
	 * @param inclination Pointer to the place to store the inclination.
	 * @return 0 when call succeded.
	 */
	int32 (*inclination)(struct device* dev, uint32* inclination);
};

/**
 * \brief Inclitiometer-interface.
 */
struct inclitiometer {
	struct inclitiometer_class_fops* fops;
	struct device* dev;
};

extern struct driver_class inclitiometer_class;


/**
 * \brief Function for getting a struct inclitiometer for a numbered inclitiometer.
 * @param idx Inclitiometer-number
 * @param inclitiometer Pointer to the ::inclitiometer to store the ::inclitiometer_class_fops and ::device.
 * @return 0 when call suceded.
 */
static inline int acquire_inclitiometer(int idx, struct inclitiometer* inclitiometer) {
	return acquire_driver_class_fops(&inclitiometer_class, &inclitiometer->dev, (void**)&inclitiometer->fops, idx);
}

/**
 * Application function for the inclitiometer_class_fops::inclination.
 * @param incl The ::inclitiometer to use.
 * @param inclination Pointer to where to store the resulting inclination.
 * @return 0 when call suceded.
 */
static inline int get_inclination(struct inclitiometer* incl, uint32* inclination) {
	return incl->fops->inclination(incl->dev,inclination);
}


#endif
