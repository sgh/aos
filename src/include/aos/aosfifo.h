#ifndef AOSFIFO_H
#define AOSFIFO_H

struct aos_fifo {
	unsigned int putidx;
	unsigned int getidx;
	unsigned int size;
	void* data;
};

void aos_fifo_init(struct aos_fifo* fifo, void* data, int len);

int aos_fifo_read(struct aos_fifo* fifo, void* dst, int len);

int aos_fifo_write(struct aos_fifo* fifo, void* src, int len);

#endif
