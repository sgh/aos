#ifndef MM_H
#define MM_H

#include <types.h>

void aos_init_mm(void* start, void* end);
void *malloc(uint16_t size);
void free(void* ptr);

#endif
