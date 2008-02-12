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
	bin(0,0,0,0,0,0,0,0),
	bin(0,0,0,0,0,0,0,0),
	bin(0,0,0,0,0,0,0,0),
	bin(0,0,0,0,0,0,0,0),
	bin(0,0,0,0,0,0,0,0),
	bin(0,0,0,0,0,0,0,0),
	bin(0,0,0,0,0,0,0,0),
};


void scancode_press(uint32_t scancode) {
	printf("press: %d  ", scancode);
}

void scancode_release(uint32_t scancode) {
	printf("release: %d  ", scancode);
}


void register_keyscan(uint8_t keyscan) {
	static uint8_t last_keyscan;
	uint32_t bit;
	uint32_t scancode = 1;
	uint8_t press = keyscan & ~last_keyscan;
	uint8_t release = last_keyscan & ~keyscan;
	last_keyscan = keyscan;
	
	printf("keyscan:0x%02X ", keyscan);
	
	scancode = 1;
	while (press || release) {
		scancode++;
		if (press & 1)
			scancode_press(scancode);
		press >>= 1;

		if (release & 1)
			scancode_release(scancode);
		release >>= 1;
	}

	printf("\n");
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
