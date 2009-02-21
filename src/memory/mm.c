/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#define AOS_KERNEL_MODULE

// #define INTEGRITY_CHECK
// #define PCDEBUG

#ifndef PCDEBUG
#include <aos/kernel.h>
#include <aos/assert.h>
#else
#include <assert.h>
#endif

#include <string.h>
#include <stdlib.h>

#include <aos/bits.h>
#include <aos/macros.h>
#include <aos/mm.h>




/* MM defines */
#define FREE_BIT 0x8000
#define SIZE_BIT 0x7FFF
#define UPPER_BYTE 0xFF00
#define LOWER_BYTE 0x00FF

// uint32_t memory_size = 0;
// uint32_t largest_segmentnum = 0;	

/* MM status variables */
static uint8_t* mm_start;
static uint8_t* mm_end;
static volatile uint8_t schedlock = 0;

typedef struct mm_header mm_header_t;

/**
 * \brief Housekeeping structure used for each memory-allocation
 */
struct PACKED mm_header {
	uint8_t  free;
	uint8_t  ________reserved;
	uint16_t size;
};

#ifndef PCDEBUG
#define boundary4_assert(val) sys_assert( ((uint32_t)(val) & 0x3) == 0)
#else
#define boundary4_assert(val) assert( ((uint32_t)(val) & 0x3) == 0)
#endif

#ifndef PCDEBUG
static_assert(sizeof(struct mm_header) == 4, MM_HEADER_MUST_BE_4_BYTES);

/* Memory-functions */
_syscall2(void, aos_mm_init, void*,  start, void*, end);
_syscall1(void*, malloc, size_t, size);
_syscall1(void, free, void*, free);
_syscall1(void, mmstat, struct mm_stat*, stat);
#else
	#define sched_lock()
	#define sched_unlock()
#endif

void mm_schedlock(uint8_t allowlock) {
	schedlock = allowlock;
}

void sys_mmstat(struct mm_stat* stat) {
	int segment = 0;
	uint8_t* ptr = (unsigned char*)mm_start;
	mm_header_t* header;
	uint32_t total_size = 0;

	memset(stat, 0, sizeof(struct mm_stat));
	
	if (schedlock)
		sched_lock();

	do {
		header = (mm_header_t*)ptr;
		total_size += header->size + sizeof(mm_header_t);
#ifdef PCDEBUG
		printf("Segment %d: %4d bytes (%s)\n",segment,header->size,header->free?"F":"U");
#endif

		if (header->free)
			stat->free += header->size;
		else {
			stat->used += header->size;
			stat->mcount++;
		}
		
		ptr = ptr + header->size + sizeof(mm_header_t);

		segment++;
	} while (ptr<mm_end);

	stat->overhead = segment * sizeof(struct mm_header);
	stat->size = stat->free + stat->used + stat->overhead;
	
	// Check for corruption. Of cause only done when fetching stats, but
	// corruptions may not be immediately fatal.
	assert(stat->size == (mm_end - mm_start));
	
	if (schedlock)
		sched_unlock();

#ifdef PCDEBUG
	printf("Total size : %4d\n",total_size);
// 	printf("Largest segmentnum: %d\n", largest_segmentnum);
// 	assert(total_size == memory_size);
	printf("\n");
#endif
}

void sys_aos_mm_init(void* start, void* end) {
	struct mm_header* head;
	uint32_t addr;

#ifdef PCDEBUG
	printf("Initializing mm\n");
#endif
	addr = (uint32_t)start;
	if (addr&0x3) {
		addr += 4;
		addr -= addr&0x3;
	}
	start = (void*)addr;

	addr = (uint32_t)end;
	addr -= addr&0x3;
	end = (void*)addr;

	boundary4_assert(start);
	boundary4_assert(end);
	
	mm_start =  start;
	mm_end   = end;
	memset(mm_start, 0x0, end-start);

	head = (struct mm_header*)mm_start;
	head->free = 1;
	head->size = (end-start) - sizeof(mm_header_t);
}

void* sys_malloc(size_t size)
{
	uint32_t segmentsize;
	uint8_t* ptr = (uint8_t*)mm_start;
	mm_header_t* header;
	mm_header_t* next_header;
	
	// Only allocate on 4 byte boundaroes
	if (size & 0x3) {
		size &= ~0x3;
		size += 4;
	}

//  	mm_status();
	if (schedlock)
		sched_lock();

	do {
		header = (mm_header_t*)ptr;
		

		if (header->free == 1) {
			/* Join Segments */
			next_header = (mm_header_t*)(ptr + header->size + sizeof(mm_header_t));

			while ((uint8_t*)next_header<mm_end && next_header->free==1) {
	// 			printf("Joining segments\n");
				header->size += next_header->size + sizeof(mm_header_t);
				next_header = (uint8_t*)(next_header) + next_header->size + sizeof(mm_header_t);
			}
		
			if (header->size>=size) {
				segmentsize = header->size;
	// 			printf("Allocating %d bytes on offset %d\n",size,(unsigned char*)ptr-mm_start+sizeof(mm_header_t));
				header->free = 0;
				header->size = size;
			
				/* If there is no room left for new mm_header after allocation */
				if (segmentsize - size < sizeof(mm_header_t))
					header->size = segmentsize;
				else { /* IF there is space free for at least a zero-byte segment */
					header = (mm_header_t*) (ptr + sizeof(mm_header_t) + size);
					if ((uint8_t*)header<mm_end) {
						header->free = 1;
						header->size = segmentsize - size - sizeof(mm_header_t);
					}
				}

#ifdef INTEGRITY_CHECK
				// Run statistics to verify integrity
				struct mm_stat mmstat;
				sys_mmstat(&mmstat);
#endif

				if (schedlock)
					sched_unlock();

				return ptr + sizeof(mm_header_t);
			}
		}
		ptr += sizeof(mm_header_t) + header->size;
	} while ((uint8_t*)ptr<mm_end);
// 	printf("Not enough memory\n");
// 	mm_status();
	
// 	AOS_WARNING("Out-Of-Memory"); // Call hook-function
const int  OUT_OF_MEMORY = 0;
#ifndef PCDEBUG
	sys_assert(OUT_OF_MEMORY);
#endif
	
	return NULL; // Out-Of-Memory
}

void sys_free(void* segment) {
	mm_header_t* header =  segment - sizeof(mm_header_t);
	uint8_t *ptr = (uint8_t*)header;
	mm_header_t* prev_header = NULL;

// 	mm_status();
	
	if (schedlock)
		sched_lock();

	header->free=1;
	do {
		header = (mm_header_t*)ptr;
		if (header->free==1 && prev_header && prev_header->free==1) {
			prev_header->size += header->size + sizeof(mm_header_t);
		}	else
			prev_header = header;
		ptr += header->size + sizeof(mm_header_t);
	}	while (header->free==1 && ptr<=(uint8_t*)mm_end - sizeof(mm_header_t));	

#ifdef INTEGRITY_CHECK
	// Run statistics to verify integrity
	struct mm_stat mmstat;
	sys_mmstat(&mmstat);
#endif

	if (schedlock)
		sched_unlock();
// 	mm_status();
}


#ifdef PCDEBUG

static char  memory[1024*32];

#define MEM_POOL_SIZE 1000

int main(int argc, char** argv) {
 	void *ptr[MEM_POOL_SIZE];
	long size;
	struct timeval t;
	void* mem[MEM_POOL_SIZE];
	int i;
	unsigned int timer = 0;
	unsigned char leftright = 0;
	struct mm_stat mmstat;
	int malloccount = 1000000;

	assert(sizeof(struct mm_header) == 4);

	gettimeofday(&t,NULL);
	
	sys_aos_mm_init((void*)memory,(void*)(memory+sizeof(memory)));
	sys_mmstat(&mmstat);
	
	srandom(t.tv_usec);
	
	for (i=0; i<MEM_POOL_SIZE; i++)
		mem[i] = 0;
	
	//mm_status();
	while (malloccount--) {
		void* ptr;
		
		size = random()%(sizeof(memory)>>3)+1;
		ptr = sys_malloc(size);
		if (ptr) { // If alloc succeded place it in the alloc-pool
// 			printf("Alloc %d\n",size);
			for (i=0 ; i<MEM_POOL_SIZE; i++) {
				if (mem[i] == 0) {
					mem[i] = ptr;
					break;
				}
			}
		} else { // Free a segment if alloc failed
			leftright ^= 1;
			if (leftright) {
				for (i=0 ; i<MEM_POOL_SIZE; i++) {
					if (mem[i] != 0) {
// 						printf("Free idx %d\n",i);
						sys_free(mem[i]);
						mem[i] = 0;
						break;
					}
				}
			} else {
				for (i=MEM_POOL_SIZE-1 ; i>=0; i--) {
					if (mem[i] != 0) {
// 						printf("Free idx %d\n",i);
						sys_free(mem[i]);
						mem[i] = 0;
						break;
					}
				}
			}
		}
		
		if (timer == 100000) {
// 			sys_mmstat(&mmstat);
// 			usleep(100000);
			timer = 0;
		} else
			timer++;
	}
	
	return 0;
}
#endif
