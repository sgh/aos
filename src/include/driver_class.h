#ifndef DRIVER_CLASS_H
#define DRIVER_CLASS_H

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
