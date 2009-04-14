
#include <stdint.h>
#include <string.h>

#ifdef __linux__
#include "../include/aos/aosfifo.h"
// #define DEBUG_INPUT
#else
#include <aos/aosfifo.h>
#endif

#ifdef DEBUG_INPUT
#include <stdio.h>
#endif

int aos_fifo_freespace(const struct aos_fifo* fifo) {
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

int aos_fifo_freespace_to_end(const struct aos_fifo* fifo) {
	int pidx = fifo->putidx;
	int gidx = fifo->getidx;
	int retval;

	if (gidx > pidx)
		retval = (gidx - pidx);
	else
		 retval = fifo->size - pidx;

	return retval;
}

int aos_fifo_avail_to_end(const struct aos_fifo* fifo) {
	int pidx = fifo->putidx;
	int gidx = fifo->getidx;
	int retval;

	if (gidx > pidx)
		retval = (fifo->size - gidx);
	else
		retval = (pidx - gidx);

	return retval;
}

int aos_fifo_data_avail(const struct aos_fifo* fifo) {
	int pidx = fifo->putidx;
	int gidx = fifo->getidx;
	int retval;

	if (pidx < gidx)
		retval = (fifo->size - gidx + pidx);
	else
		retval = (pidx - gidx);
	
	return retval;
}

void aos_fifo_init(struct aos_fifo* fifo, void* data, int len) {
	fifo->putidx = 0;
	fifo->getidx = 0;
	fifo->size = len;
	fifo->data = data;
}

void memcpy2(char* dst, char* src, int len) {
	memcpy(dst,src,len);
}

int aos_fifo_read(struct aos_fifo* fifo, void* dst, int selement, int nelements) {
	unsigned int getidx = fifo->getidx;
	int total_avail = aos_fifo_data_avail(fifo);
	int total_size = nelements * selement;
	int total_avail_to_end = aos_fifo_avail_to_end(fifo);

	if (total_size > total_avail) {
		nelements = total_avail / selement;
		total_size = nelements * selement;
		if (total_avail_to_end > total_size)
			total_avail_to_end = total_size;
	}

	int copy_to_end = total_size;
	int copy_from_start = 0;

	if (total_size > total_avail_to_end) {
		copy_to_end = total_avail_to_end;
		copy_from_start = total_size - total_avail_to_end;
	}

#ifdef DEBUG_INPUT
	printf("Reading %d*%d bytes %d,%d...", nelements, selement, total_avail, total_avail_to_end);
	printf("copy(%d,%d) ", copy_to_end, copy_from_start);
	fflush(0);
#endif

	// First copy to end
	memcpy2(dst, fifo->data + getidx, copy_to_end);

	// Then copy from start
	memcpy2(dst + copy_to_end, fifo->data, copy_from_start);

	getidx += total_size;

	if (getidx >= fifo->size)
		getidx -= fifo->size;

	fifo->getidx = getidx;
#ifdef DEBUG_INPUT
		printf(" :: %d bytes\n", selement*nelements);
		fflush(0);
#endif

	return nelements;
}

int aos_fifo_write(struct aos_fifo* fifo, void* src, int selement, int nelements) {
	unsigned int putidx = fifo->putidx;
	int free_space = aos_fifo_freespace(fifo);
	int free_space_to_end = aos_fifo_freespace_to_end(fifo);
	int total_size = nelements * selement;
	int copy_from_start = 0;

	// Limit the total amount of data to be copied. If the free space is not enought
	// Then just clamp the number of elements to copy
	if (free_space < total_size) {
		nelements = free_space / selement;
		total_size = nelements * selement;
	}

#ifdef DEBUG_INPUT
	printf("Adding %d*%d bytes ...", nelements, selement);
	fflush(0);
#endif

	int copy_to_end = total_size;

	if (copy_to_end > free_space_to_end) {
		copy_to_end = free_space_to_end;
		copy_from_start = total_size - copy_to_end;
	}

	#ifdef DEBUG_INPUT
		printf("Copy (to end, from start) : (%d bytes, %d bytes) ", copy_to_end, copy_from_start);
		fflush(0);
	#endif

	// First copy to end
	memcpy2(fifo->data + putidx, src, copy_to_end);

	// Then copy from start
	memcpy2(fifo->data, src + copy_to_end, copy_from_start);

	putidx += total_size;

	if (putidx >= fifo->size)
		putidx -= fifo->size;
	
	fifo->putidx = putidx;

#ifdef DEBUG_INPUT
	printf(" :: %d bytes\n", total_size);
	fflush(0);
#endif

	return nelements;
}

#ifdef __linux__

#include <assert.h>

void fifotest() {
	unsigned char buf[32];
	struct aos_fifo testfifo;
	unsigned char sbuf[8];
	unsigned char dbuf[8];

	// First test reading from empty list
	aos_fifo_init(&testfifo, buf, sizeof(buf));
	assert( aos_fifo_read(&testfifo, sbuf, 1, 1) == 0);

	// Now test writing one byte and reading it back
	sbuf[0] = sbuf[1] = 'a';
	dbuf[0] = dbuf[1] = 'b';
	assert( aos_fifo_write(&testfifo, sbuf, 1, 1) == 1 );
	assert( aos_fifo_read(&testfifo, dbuf, 1, 1)  == 1 );
	assert( dbuf[0] == 'a' && dbuf[1] == 'b' );
	dbuf[0] = dbuf[1] = '_';
	assert( aos_fifo_read(&testfifo, dbuf, 1, 1)  == 0 );
	assert( dbuf[0] == '_' && dbuf[1] == '_' );

	// Now test writing one chunk of 2 bytes and reading it back 
	sbuf[0] = sbuf[1] = sbuf[2] = 'a';
	dbuf[0] = dbuf[1] = dbuf[2] = 'b';
	assert( aos_fifo_write(&testfifo, sbuf, 2, 1) == 1 );
	assert( aos_fifo_read(&testfifo, dbuf, 2, 1)  == 1 );
	assert( dbuf[0] == 'a' && dbuf[1] == 'a' && dbuf[2] == 'b' );
	dbuf[0] = dbuf[1] = dbuf[2] = '_';
	assert( aos_fifo_read(&testfifo, dbuf, 1, 1)  == 0 );
	assert( dbuf[0] == '_' && dbuf[1] == '_' && dbuf[2] == '_' );

	// Now test writing one chunk of 2 bytes and reading it back in a 3 bytes chunk
	// The reading the real 2 bytes chunk
	sbuf[0] = sbuf[1] = sbuf[2] = 'a';
	dbuf[0] = dbuf[1] = dbuf[2] = 'b';
	assert( aos_fifo_write(&testfifo, sbuf, 2, 1) == 1 );
	assert( aos_fifo_read(&testfifo, dbuf, 3, 1)  == 0 );
	assert( dbuf[0] == 'b' && dbuf[1] == 'b' && dbuf[2] == 'b' );
	assert( aos_fifo_read(&testfifo, dbuf, 2, 1)  == 1 );
	assert( dbuf[0] == 'a' && dbuf[1] == 'a' && dbuf[2] == 'b' );
	dbuf[0] = dbuf[1] = dbuf[2] = '_';
	assert( aos_fifo_read(&testfifo, dbuf, 1, 1)  == 0 );
	assert( dbuf[0] == '_' && dbuf[1] == '_' && dbuf[2] == '_' );

	// Now write and read 5 byte chunks
	int i;
	sbuf[0] = sbuf[1] = sbuf[2] = sbuf[3] = sbuf[4] = sbuf[5] ='a';
	dbuf[0] = dbuf[1] = dbuf[2] = dbuf[3] = dbuf[4] = dbuf[5] ='b';
	for (i=0; i<1024*1024*10; i++) {
		dbuf[0] = 'b';
		assert( aos_fifo_write(&testfifo, sbuf, 5, 1) == 1 );
		assert( aos_fifo_read(&testfifo, dbuf, 5, 1)  == 1 );
		assert( dbuf[0]=='a' && dbuf[1]=='a' && dbuf[2]=='a' && dbuf[3]=='a' && dbuf[4]=='a' && dbuf[5]=='b' );
	}

}

int main() {
	unsigned char buf[32];
	struct aos_fifo testfifo;
	unsigned char sbuf[8];

	fifotest();
	return 0;
	aos_fifo_init(&testfifo, buf, sizeof(buf));

	aos_fifo_write(&testfifo, sbuf, sizeof(sbuf)/2, 2 );

// 	aos_fifo_read(&testfifo, sbuf, sizeof(sbuf)/2, 2);
	aos_fifo_write(&testfifo, sbuf, sizeof(sbuf)/2, 2 );
	aos_fifo_write(&testfifo, sbuf, sizeof(sbuf)/2, 2 );
	aos_fifo_write(&testfifo, sbuf, sizeof(sbuf)/2, 2 );
	aos_fifo_write(&testfifo, sbuf, sizeof(sbuf)/2, 2 );


}
#endif
