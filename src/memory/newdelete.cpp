#include <stdlib.h>

void* operator new(size_t size) throw() {
  return malloc(size);
}


void operator delete(void* p) throw() {
  free(p);
}

void *operator new[] (size_t size) {      // for arays
  return malloc(size);
}

void operator delete[] (void* p) {
  free(p);
}

void abort(void) {
  // this function is called when an exception occurs.
  // For production code you need to log the message and go to fail-safe
  // state. You might also want to reset the CPU.
  //
  for (;;) {
	}
}
