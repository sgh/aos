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
#ifndef DRIVER_CLASS_H
#define DRIVER_CLASS_H

#include <aos_module.h>

#include <driver_core.h>
#include <types.h>
#include <list.h>


int class_fops_register(struct driver_class* cls, struct driver_class_fops* fops);

/**
 * \brief Utility function to get a pair of struct device and fops that match a given device-number in a given struct driver_class.
 * @param cls The class in question
 * @param dev Pointer to the place to store the pointer to the found device
 * @param fops Pointer to the place to store the pointer to the fops of the driver that owns the found device
 * @param devicenum The device-number to get.
 * @return 0 if device and fops are found.
 */
int acquire_driver_class_fops(struct driver_class* cls, struct device** dev, void** fops, int devicenum);


#endif
