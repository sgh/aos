#ifndef AOS_AOSFIFO_H
#define AOS_AOSFIFO_H

#ifdef __cplusplus
extern "C" {
#endif

struct aos_fifo {
	unsigned int putidx;
	unsigned int getidx;
	unsigned int size;
	unsigned char* data;
};

void aos_fifo_init(struct aos_fifo* fifo, void* data, unsigned int size);

int aos_fifo_read(struct aos_fifo* fifo, unsigned char* dst, unsigned int len);

int aos_fifo_write(struct aos_fifo* fifo, unsigned char* src, unsigned int len);

#ifdef __cplusplus
}
#endif

#endif
