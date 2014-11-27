/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/#include <stdint.h>

/**
 * \brief Timer-value when usage stats where last recorded
 */
static uint32_t last_usage_point;


void cpuusage_system() {
}

void cpuusage_user() {
	
}

void cpuusage_idle() {
}

int main() {
	int i;
	float usage = 0;

	for (i=0; i<10; i++) {
		usage = usage * 0.9 +  (i&1)*0.1;
		printf("%f\n", usage);
	}

}
