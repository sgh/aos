#ifndef MM_H
#define MM_H

#include <types.h>

void mm_init(void* start, unsigned short len);
void *malloc(uint16_t size);
void free(void* ptr);

#endif
