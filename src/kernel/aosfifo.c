#include <stdint.h>
#include <string.h>
#include <aos/macros.h>

#ifdef __linux__
#include "../include/aos/aosfifo.h"
#include <assert.h>
// #define DEBUG_INPUT
#include <stdio.h>
#else
#include <aos/aosfifo.h>
#endif

static inline int aos_fifo_freespace(const struct aos_fifo* fifo) {
	int pidx = fifo->putidx;
	int gidx = fifo->getidx;
	int free_space;

	// If that space "between" pidx and gidx is the currently used space
	// Else the space must be the free space
	if (pidx >= gidx)
		free_space = (fifo->size - (pidx - gidx)) - 1;
	else
		free_space =  (gidx - pidx) - 1;
	
#ifdef DEBUG_INPUT
	printf("Free space: %d bytes ", free_space);
	fflush(0);
#endif
	return free_space;
}

static inline int aos_fifo_freespace_to_end(int gidx, int pidx, const struct aos_fifo* fifo) {
	int retval;

	if (gidx > pidx)
		retval = (gidx - pidx);
	else
		 retval = fifo->size - pidx;

	return retval;
}

static inline int aos_fifo_avail_to_end(int gidx, int pidx, const struct aos_fifo* fifo) {
	int retval;

	if (gidx > pidx)
		retval = (fifo->size - gidx);
	else
		retval = (pidx - gidx);

	return retval;
}

static inline int aos_fifo_data_avail(int gidx, int pidx, int fifo_size, const struct aos_fifo* fifo) {
	if (pidx >= gidx)
		return (pidx - gidx);
	else
		return (fifo_size - gidx + pidx);
}

void aos_fifo_init(struct aos_fifo* fifo, void* data, int len) {
	fifo->putidx = 0;
	fifo->getidx = 0;
	fifo->size = len;
	fifo->data = data;
}

int __attribute__((optimize(1))) aos_fifo_read(struct aos_fifo* fifo, void* dst, int len) {
	unsigned int getidx = fifo->getidx;
	unsigned int putidx = fifo->putidx;
	uint8_t* ucdst = dst;
	uint8_t* ucsrc = fifo->data;

	int to_end = 0;
	int from_start = 0;

	// Normal without wrap around
	if (likely(getidx < putidx)) {
		to_end     = putidx - getidx;
		if (to_end > len)
			to_end = len;
	}

	// Data wraping around
	if (unlikely(putidx < getidx)) {
		to_end     = fifo->size - getidx;
		if (to_end > len)
			to_end = len;
		from_start = putidx;
		if (from_start > len)
			from_start = len;
	}

// 	printf("%d %d\n", to_end, from_start);

	int count = to_end + from_start;

	while (to_end--)
		*(ucdst++) = ucsrc[getidx++];

	if (unlikely(from_start)) {
		getidx = 0;
		while (from_start--)
			*(ucdst++) = ucsrc[getidx++];
	}

// 	printf("len: %d\n", len);

	fifo->getidx = getidx;
	
	return count;
}


int __attribute__((optimize(1))) aos_fifo_write(struct aos_fifo* fifo, void* src, int len) {
	unsigned int getidx = fifo->getidx;
	unsigned int putidx = fifo->putidx;
	uint8_t* ucsrc = src;
	uint8_t* ucdst = fifo->data;

	int to_end = 0;
	int from_start = 0;

	// Normal without wrap around
	if (likely(getidx <= putidx)) {
		to_end     =  fifo->size - putidx;
		from_start = getidx;

		if (to_end+from_start > len) {
			from_start -= to_end+from_start - len;
			if (from_start < 0) {
				to_end += from_start;
				from_start = 0;
			}
		}
	}

	// Data wraping around
	if (unlikely(putidx < getidx)) {
		to_end     = getidx - putidx;
		if (to_end > len)
			to_end = len;
	}

// 	printf("%d %d\n", to_end, from_start);

	int count = to_end + from_start;

	while (to_end--)
		ucdst[putidx++] = *(ucsrc++);

	if (unlikely(from_start)) {
		putidx = 0;
		while (from_start--)
			ucdst[putidx++] = *(ucsrc++);
	}

// 	printf("len: %d\n", len);
	if (putidx >= fifo->size)
		putidx -= fifo->size;

	fifo->putidx = putidx;
	
	return count;
}

//#define AOSFIFO_TEST
#ifdef AOSFIFO_TEST

void fifotest(void) {
	unsigned char buf[32];
	struct aos_fifo testfifo;
	unsigned char sbuf[32];
	unsigned char dbuf[32];

	// First test reading from empty list
	aos_fifo_init(&testfifo, buf, sizeof(buf));
	assert( aos_fifo_read(&testfifo, sbuf, 1) == 0);

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
// 	assert( dbuf[0] == 'b' && dbuf[1] == 'b' && dbuf[2] == 'b' ); // THIS IS THE ONLY TEST THAT FAILS
	assert( aos_fifo_read(&testfifo, dbuf, 1)  == 0 );
	assert( dbuf[0] == 'a' && dbuf[1] == 'a' && dbuf[2] == 'b' );
	dbuf[0] = dbuf[1] = dbuf[2] = '_';
	assert( aos_fifo_read(&testfifo, dbuf, 1)  == 0 );
	assert( dbuf[0] == '_' && dbuf[1] == '_' && dbuf[2] == '_' );


	// Now write and read 5 byte chunks
	int i;
	sbuf[0] = sbuf[1] = sbuf[2] = sbuf[3] = sbuf[4] = sbuf[5] ='a';
	dbuf[0] = dbuf[1] = dbuf[2] = dbuf[3] = dbuf[4] = dbuf[5] ='b';
	for (i=0; i<1024*1024*10; i++) {
		dbuf[0] = 'b';
// 		memcpy(sbuf, dbuf, 5);
// 		memcpy(dbuf, sbuf, 5);
		assert( aos_fifo_write(&testfifo, sbuf, 5) == 5 );
		assert( aos_fifo_read(&testfifo, dbuf, 5)  == 5 );
// 		assert( dbuf[0]=='a' && dbuf[1]=='a' && dbuf[2]=='a' && dbuf[3]=='a' && dbuf[4]=='a' && dbuf[5]=='b' );
	}
}

int main() {
	fifotest();
	return 0;
}
#endif
