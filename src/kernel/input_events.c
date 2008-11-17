#include <stdint.h>
#include <stdio.h>

#warning implement eventqueue here

void dispatch_keypress(int scancode) {
	printf("Keypress %d\n", scancode);
}

void dispatch_keyrelease(int scancode) {
	printf("Keyrelease %d\n", scancode);
}
