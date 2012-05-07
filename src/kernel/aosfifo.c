#include <stdint.h>
#include <string.h>
#include <aos/macros.h>

#ifdef __linux__
#include "../include/aos/aosfifo.h"
#include <assert.h>
//#define AOSFIFO_TEST
#include <stdio.h>
#else
#include <aos/aosfifo.h>
#endif

// Unit test: gcc -o aostest aosfifo.c -I ../include/ && ./aostest

void aos_fifo_init(struct aos_fifo* fifo, void* data, unsigned int len) {
	fifo->putidx = 0;
	fifo->getidx = 0;
	fifo->size = len;
	fifo->data = data;
}

// static inline _memcpy(unsigned char* dst, unsigned char* src, unsigned int len) {
// 	while (len--)
// 		*(dst++) = *(src++);
// }

int __attribute__((optimize(3))) aos_fifo_read(struct aos_fifo* fifo, unsigned char* dst, unsigned int len) {
	unsigned int l;

	len = min(len, fifo->putidx - fifo->getidx);

	l = min(len, fifo->size - fifo->getidx);

	// Ensure correct len when data wraps
	if (fifo->putidx < fifo->getidx)
		len = min(len, l + fifo->putidx);

	if (likely(l))
		memcpy(dst, &fifo->data[fifo->getidx], l);

	if (unlikely(len - l))
		memcpy(dst + l, &fifo->data[ (fifo->getidx + l) % fifo->size ], len - l);

// 	fifo->getidx = (fifo->getidx + len) % fifo->size;

	if (unlikely((fifo->getidx + len)) >= fifo->size)
		fifo->getidx = (fifo->getidx + len) - fifo->size;
	else
		fifo->getidx = (fifo->getidx + len);

	return len;
}


int __attribute__((optimize(3))) aos_fifo_write(struct aos_fifo* fifo, unsigned char* src, unsigned int len) {
	unsigned int l;

	len = min(len, fifo->size - fifo->putidx + fifo->getidx - 1);

	l = min(len, fifo->size - fifo->putidx);

	if (likely(l))
		memcpy(&fifo->data[fifo->putidx], src, l);
	
	if (unlikely(len - l))
		memcpy(fifo->data, src + l, len - l);

// 	fifo->putidx = (fifo->putidx + len) % fifo->size;

	if (unlikely((fifo->putidx + len)) >= fifo->size)
		fifo->putidx = (fifo->putidx + len) - fifo->size;
	else
		fifo->putidx = (fifo->putidx + len);

	return len;
}


#ifdef AOSFIFO_TEST

void fifotest(void) {
	unsigned char buf[32];
	struct aos_fifo testfifo;
	unsigned char sbuf[32];
	unsigned char dbuf[32];
	int i;

	// First test reading from empty list
	aos_fifo_init(&testfifo, buf, sizeof(buf));
#define NORMAL_TESTS
#ifdef NORMAL_TESTS
	assert( aos_fifo_read(&testfifo, sbuf, 1) == 0);

	// Fill the buffer with 31 bytes
	for (i=0; i<31; i++)
		assert( aos_fifo_write(&testfifo, sbuf, 1) == 1 );

	// Writing a byte more should return 0
	assert( aos_fifo_write(&testfifo, sbuf, 1) == 0 );

	// Read the 32 bytes back
	for (i=0; i<31; i++)
		assert( aos_fifo_read(&testfifo, dbuf, 1) == 1 );

	assert( aos_fifo_read(&testfifo, dbuf, 1) == 0 );

	// Now test writing one byte and reading it back
	sbuf[0] = sbuf[1] = 'a';
	dbuf[0] = dbuf[1] = 'b';
	assert( aos_fifo_write(&testfifo, sbuf, 1) == 1 );
	assert( aos_fifo_read(&testfifo, dbuf, 1)  == 1 );
	assert( dbuf[0] == 'a' && dbuf[1] == 'b' );
	dbuf[0] = dbuf[1] = '_';
	assert( aos_fifo_read(&testfifo, dbuf, 1)  == 0 );
	assert( dbuf[0] == '_' && dbuf[1] == '_' );

	// Now test writing one chunk of 2 bytes and reading it back 
	sbuf[0] = sbuf[1] = sbuf[2] = 'a';
	dbuf[0] = dbuf[1] = dbuf[2] = 'b';
	assert( aos_fifo_write(&testfifo, sbuf, 2) == 2 );
	assert( aos_fifo_read(&testfifo, dbuf, 1)  == 1 );
	assert( aos_fifo_read(&testfifo, dbuf+1, 1)  == 1 );
	assert( dbuf[0] == 'a' && dbuf[1] == 'a' && dbuf[2] == 'b' );
	dbuf[0] = dbuf[1] = dbuf[2] = '_';
	assert( aos_fifo_read(&testfifo, dbuf, 1)  == 0 );
	assert( dbuf[0] == '_' && dbuf[1] == '_' && dbuf[2] == '_' );

	// Now test writing one chunk of 2 bytes and reading it back in a 3 bytes chunk
	// The reading the real 2 bytes chunk
	sbuf[0] = sbuf[1] = sbuf[2] = 'a';
	dbuf[0] = dbuf[1] = dbuf[2] = 'b';
	assert( aos_fifo_write(&testfifo, sbuf, 2) == 2 );
	assert( aos_fifo_read(&testfifo, dbuf, 3)  == 2 );
	assert( dbuf[0] == 'a' && dbuf[1] == 'a' && dbuf[2] == 'b' );
	assert( aos_fifo_read(&testfifo, dbuf, 1)  == 0 );
	assert( dbuf[0] == 'a' && dbuf[1] == 'a' && dbuf[2] == 'b' );
	dbuf[0] = dbuf[1] = dbuf[2] = '_';
	assert( aos_fifo_read(&testfifo, dbuf, 1)  == 0 );
	assert( dbuf[0] == '_' && dbuf[1] == '_' && dbuf[2] == '_' );

#endif
	// Now write and read 5 byte chunks
	sbuf[0] = sbuf[1] = sbuf[2] = sbuf[3] = sbuf[4] = sbuf[5] ='a';
	dbuf[0] = dbuf[1] = dbuf[2] = dbuf[3] = dbuf[4] = dbuf[5] ='b';
	for (i=0; i<1024*1024*50; i++) {
		dbuf[0] = 'b';
// 		memcpy(sbuf, dbuf, 5);
// 		memcpy(dbuf, sbuf, 5);
		assert( aos_fifo_write(&testfifo, sbuf, 12) == 12 );
		assert( aos_fifo_read(&testfifo, dbuf, 6)  == 6 );
		assert( aos_fifo_read(&testfifo, dbuf, 7)  == 6 );
// 		assert( dbuf[0]=='a' && dbuf[1]=='a' && dbuf[2]=='a' && dbuf[3]=='a' && dbuf[4]=='a' && dbuf[5]=='b' );
	}
}

int main() {
	fifotest();
	return 0;
}
#endif
