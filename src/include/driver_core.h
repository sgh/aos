#ifndef CORE_H
#define CORE_H

#include <aos_module.h>

#include <gpio.h>
#include <list.h>
#include <macros.h>
#include <deviceids.h>
#include <errno.h>

#define DEVICE_NAME_SIZE 32
#define BUS_ID_SIZE 32

/**
 * \brief This list holds all the devices attached to the system.
 */
extern struct list_head global_device_list;

/**
 * \brief This list holds all the busses in the system.
 */
extern struct list_head global_bus_list;

/**
 * \brief This list holds all the drivers in the system.
 */
extern struct list_head global_driver_list;

/**
 * \brief This list holds all the driver-classes in the system.
 */
extern struct list_head global_class_list;


struct bus_type;
struct device;
struct device_driver;


/**
 * \brief Represents a single device in the system
 */
struct device {
	struct list_head g_list;			/**< \brief Node in the global device-list. */
	struct list_head bus_list;		/**< \brief Node in the bus' device-list. */
	struct list_head driver_list;	/**< \brief Node in the drivers' device-list. */
	
	uint8_t device_id;							/**< \brief Device-ID. Since PnP is not available, ID's must be hardcoded. */
	
	struct device_driver* driver;	/**< \brief The driver that owns this device. */
	
	char name[DEVICE_NAME_SIZE];	/**< \brief Name of the device. */
	char bus_id[BUS_ID_SIZE];			/**< \brief ID on the bus. Unique for every bus. */
	
 	struct bus_type* bus;					/**< \brief The bus this device is on. */
	void* driver_data;						/**< \brief Driver-specific data. Drivers must use this for whatever per device stats. */
	void* platform_data;					/**< \brief Platform specific-data. This will typically be a pointer to a struct gpio. */
	void* class_data;							/**< \brief Class specific-data. */
	
	uint8_t current_state;					/**< \brief The current state of the device. */
};


/**
 * \brief Represents a bus in the system.
 */
struct bus_type {
	struct list_head g_list;									/**< \brief Node in global devicelist. */
	struct list_head drivers;									/**< \brief List of owned drivers. */
	char name[BUS_ID_SIZE];
	
	/**
	 * \brief Called to suspend a devices on the bus.
	 * @param dev The device to suspend
	 * @return 0 when suspend succeded. Non-zero when suspend failed.
	 */
	int (*suspend)(struct device *dev);
	
	/** \brief Called to resume a device on the bus.
	 * @param dev The device on the bus to resume
	 * @return 0 when resume succeded. Non-zero when resume failed.
	 */
	int (*resume)(struct device *dev);
	
	/**
	 * \brief Called to match a device with a given device_driver.
	 * @param dev The device to match
	 * @param drv The driver to match with
	 * @return 0 when device matched succesfull. Non-zero when match failed.
	 */
	int (*match)(struct device *dev, struct device_driver *drv);
};


/**
 * \brief A driver for at specific type of device.
 */
struct device_driver {
	char* name;
	struct list_head g_list;						/**< \brief Node in global driver list. */
	struct list_head bus_driver_list;		/**< \brief Node in bus' driver list. */
	struct list_head devices;						/**< \brief List of owned devices. */
	
	struct driver_interface* devintf;
	
	struct bus_type* bus;
	
	/**
	 * \brief Called after verifying ID-match. The function shal verify that the driver realy supports the device.
	 * @param dev The device to probe
	 * @return 0 when device probed succesfull.
	 */
	int (*probe)(struct device* dev);
	
	/**
	 * \brief Called when removing a device from the bus.
	 * @param dev Device to remove
	 * @return 0 when device matched succesfull.
	 */
	int (*remove)(struct device* dev);
	
	/**
	 * \brief Called to suspend a device on the bus.
	 * @param dev Device to suspend
	 * @return 0 when device suepended succesfull.
	 */
	int (*suspend)(struct device* dev);
	
	/**
	 * \brief Called to resume a device on the bus.
	 * @param dev Device to resume
	 * @return 0 when device resumed succesfull.
	 */
	int (*resume)(struct device* dev);
	
	/**
	 * \brief Generic ioctl-call to use when setting driver-specific settings on owned devices.
	 * @param dev Pointer to the device to execute ioctl on.
	 * @param request The actual request to execute. See the driver documentation.
	 * @return 0 when ioctl succceded.
	 */
	int (*ioctl)(struct device* dev, int request, ...);
};


/**
 * \brief Entry in the global class-list.
 *
 * There exists only one instance pr. class of this structure.
 */
struct driver_class {
	char *name;
	struct list_head g_list;						/**< \brief Node in global class list. */
	struct list_head class_fops;				/**< \brief List of driver_class_fops. */
};


/**
 * \brief This structure defines a driver implementation of a class.
 */
struct driver_class_fops {
	void* fops;											/**< \brief Class-specific function-pointers. Typecasted by the respective acquire-fuctions. */
	struct device_driver* driver;		/**< \brief Device-driver registered. */
	struct list_head node;					/**< \brief Node in driver_class::class_fops. */
};


/**
 * \brief Register a bus with the core.
 * @param bus Pointer to a struct bus_type
 */
void bus_register(struct bus_type* bus);


/**
 * \brief Register a device with the core.
 * @param dev Pointer to a struct device
 */
void device_register(struct device* dev);


/**
 * \brief Register a driver with the core.
 * @param drv Pointer to a struct device_driver
 */
void driver_register(struct device_driver* drv);


/**
 * \brief Register a driver-class with the core.
 * @param cls Pointer to a struct driver_class
 */
void class_register(struct driver_class* cls);

/*int bus_for_each_dev(struct bus_type * bus, struct device * start, void * data, int (*fn)(struct device *, void *));

int bus_for_each_drv(struct bus_type * bus, struct device_driver * start, void * data, int (*fn)(struct device_driver *, void *));*/

static __inline__ int device_ioctl(struct device* dev, int request) {
	if (!dev)
		return ENOIOCTL;
	return dev->driver->ioctl(dev,request);
}

#endif
