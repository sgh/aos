#include <aostk.h>

static const struct aostk_keyaccel* key_accelerators;

void aostk_key_accelerators(struct aostk_keyaccel* accels) {
	key_accelerators = accels;
}

void keyaccel_input(uint32_t key) {
	int i;
	if (!key_accelerators)
		return;
	for (i=0; key_accelerators[i].handler; i++) {
		if (key_accelerators[i].key == key)
			key_accelerators[i].handler(key);
	}
}
