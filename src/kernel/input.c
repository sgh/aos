#include <stdio.h>
#include <stdint.h>

#define bin(a,b,c,d,e,f,g,h) ((a)*128+(b)*64+(c)*32+(d)*16+(e)*8+(f)*4+(g)*2+(h)*1)

uint8_t keyscans[] = {
	bin(0,0,0,0,0,0,0,0),
	bin(1,0,0,0,0,0,0,0),
	bin(1,0,0,0,0,0,0,0),
	bin(1,0,0,0,0,0,0,1),
	bin(1,0,0,0,0,0,0,1),
	bin(1,0,0,0,0,0,0,0),
	bin(1,0,0,0,0,0,0,1),
	bin(1,0,0,0,0,0,1,1),
	bin(1,0,0,0,0,1,1,1),
	bin(1,0,0,0,0,0,0,0),
	bin(0,0,0,0,0,0,0,0),
	bin(0,0,0,0,0,0,0,0),
	bin(0,0,0,0,0,0,0,0),
};

static uint8_t additional_keys[2];
static uint8_t active_key;
static uint8_t pressed_idx;

void pressedlist_add(uint32_t scancode) {
	pressed_idx++;
	if (pressed_idx > 1)
		pressed_idx = 0;

	additional_keys[pressed_idx] = active_key = scancode;
}

void pressedlist_remove(uint32_t scancode) {
	int i;
	for (i=0; i<2; i++) {
		if (additional_keys[i] == scancode)
			additional_keys[i] = 0;
	}
	
	if (active_key == scancode)
		active_key = 0;
}

void scancode_press(uint32_t scancode) {

	printf("press:   %4d ", scancode);

	pressedlist_add(scancode);
}

void scancode_release(uint32_t scancode) {
	printf("release: %4d ", scancode);

	pressedlist_remove(scancode);
}


void register_keyscan(uint8_t keyscan) {
	static uint8_t last_keyscan;
	int i;
	uint32_t bit;
	uint32_t scancode = 1;
	uint8_t press = keyscan & ~last_keyscan;
	uint8_t release = last_keyscan & ~keyscan;
	last_keyscan = keyscan;
	
	printf("keyscan:0x%02X ", keyscan);
	
	// Run through released keys
// 	scancode = 1;
	for (scancode = 1; release ; release >>= 1) {
		if (release & 1)
			scancode_release(scancode);
		scancode++;
	}
	
	for (scancode = 1; press ; press >>= 1) {
		if (press & 1)
			scancode_press(scancode);
		scancode++;
	}
	
	printf(" %d[", active_key);

	for (i=0; i<2; i++)
		printf("%d ", additional_keys[i]);

	printf("]\n");
}


// unsigned bit_count(unsigned n)
//  {
//  unsigned count;
//  
//  for (count = 0; n; n &= n - 1) {
// 	 printf("n: %d\n",n);
//  	++count;
//  }
//  return count;
// }



int main(int argc, const char* argv[]) {
	int i;
	for (i = 0; i<sizeof(keyscans); i++)
		register_keyscan(keyscans[i]);
	
// 	printf("bx: %d\n", bit_count(64+5));
}
