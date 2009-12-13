#include <string.h>
#include <arm/lpc23xx.h>
#include <bits.h>
#include <stdint.h>

// extern "C" void _fini(void) {
// }

// extern "C" void _init(void) {
// }
#include <list>

using namespace std;

class Led {
	list<int> l;
	uint8_t _state;
	uint32_t _bits;
	

public:

	Led(uint32_t b) : _state(0), _bits(b) {
		l.push_back(0);
	}

	void toggle(void) {
		_state ^= 1;
		if (_state)
			FIO2CLR = _bits;
		else
			FIO2SET = _bits;

	}

};

class ChildLed : Led {

public:
	ChildLed(void) : Led(BIT4) {
	}

	void flip(void) {
		toggle();
	}
};


// void __attribute__((constructor)) testhest(void) {
// 	for(;;) {};
// }




int main(void) {
	volatile int i;

	Led my_ledA(BIT0);
	Led my_ledB(BIT7);
	ChildLed b4;
	
	PINSEL10 = 0;           /* Disable ETM interface, enable LEDs */
	FIO2DIR  = 0x000000FF;  /* P2.0..7 defined as Outputs         */
	FIO2MASK = 0x00000000;
	FIO2SET  = BIT0;

	for (;;) {
 		for (i=0; i<400000; i++) {}
// 		FIO2SET = BIT7;
 		my_ledA.toggle();

 		for (i=0; i<400000; i++) {}
// 	 	FIO2CLR = BIT7;
 		my_ledB.toggle();

		b4.flip();
	}
	return 0;	
}
