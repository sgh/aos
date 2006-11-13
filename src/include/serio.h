#ifndef SERIO_H
#define SERIO_H

#include <driver_core.h>
#include <driver_class.h>

/**
 * \brief Function-pointers for all inclitiometers
 */
struct serio_class_fops {
	/**
	 * \brief Put a sequency of chars to the serial IO
	 * @param dev The ::device in question.
	 * @param src Pointer to the chars to send 
	 * @param len Number of bytes to send
	 * @return 0 when call succeded.
	 */
	void (*put)(struct device* dev, char* src, size_t len);
};

/**
 * \brief Serial IO interface.
 */
struct serio {
	struct serio_class_fops* fops;
	struct device* dev;
};

extern struct driver_class serio_class;


/**
 * \brief Function for getting a struct serio for a numbered serio.
 * @param idx Serio-number
 * @param inclitiometer Pointer to the ::serio to store the ::serio_class_fops and ::device.
 * @return 0 when call suceded.
 */
static inline int acquire_serio(int idx, struct serio* serio) {
	return acquire_driver_class_fops(&serio_class, &serio->dev, (void**)&serio->fops, idx);
}

/**
 * Application function for the serio_class_fops::put.
 * @param incl The ::serio to use.
 * @return 0 when call suceded.
 */
static inline void serio_put(struct serio* serio, char* src, size_t len) {
	serio->fops->put(serio->dev,src,len);
}


#endif
