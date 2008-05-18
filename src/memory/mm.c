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

#include <aos/kernel.h>
#include <string.h>
#include <aos/bits.h>
#include <aos/assert.h>


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

#define boundary4_assert(val) sys_assert( ((uint32_t)(val) & 0x3) == 0)

static_assert(sizeof(struct mm_header) == 4, MM_HEADER_MUST_BE_4_BYTES);


/* Memory-functions */
_syscall2(void, aos_mm_init, void*,  start, void*, end);
_syscall1(void*, malloc, size_t, size);
_syscall1(void, free, void*, free);
_syscall1(void, mmstat, struct mm_stat*, stat);

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
// 		printf("Segment %d: %4d bytes (%s)\n",segment,header->size,header->free?"F":"U");
//
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

// 	printf("Total size : %4d\n",total_size);
// 	printf("Largest segmentnum: %d\n", largest_segmentnum);
// 	assert(total_size == memory_size);
// 	printf("\n");
}

void sys_aos_mm_init(void* start, void* end) {
	struct mm_header* head;
	
	boundary4_assert(start);
	boundary4_assert(end);
	
	mm_start =  start;
	mm_end = end;
	memset(mm_start, 0x0, end-start);
// 	memory_size = len;

	head = (struct mm_header*)mm_start;
	head->free = 1;
	head->size = (end-start) - sizeof(mm_header_t);
// 	mm_status();
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
		next_header = (mm_header_t*)(ptr + header->size + sizeof(mm_header_t));
		
		/* Join Segments */
		if (header->free==1 && (uint8_t*)next_header<mm_end && next_header->free==1) {
// 			printf("Joining segments\n");
// 			mm_status();
			header->size += next_header->size + sizeof(mm_header_t);
// 		mm_status();
		}
		
		if (header->size>=size && header->free==1) {
			segmentsize = header->size;
// 			mm_status();
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
// 			mm_status();
			if (schedlock)
				sched_unlock();

			// Run statistics to verify integrity
			struct mm_stat mmstat;
			sys_mmstat(&mmstat);

			return ptr + sizeof(mm_header_t);
		}
		ptr += sizeof(mm_header_t) + header->size;
	} while ((uint8_t*)ptr<mm_end);
// 	printf("Not enough memory\n");
// 	mm_status();
	
// 	AOS_WARNING("Out-Of-Memory"); // Call hook-function
const int  OUT_OF_MEMORY = 0;
	sys_assert(OUT_OF_MEMORY);
	
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
		if (prev_header && header->free==1 && prev_header->free==1) {
			prev_header->size += header->size + sizeof(mm_header_t);
		}	else
			prev_header = header;
		ptr += header->size + sizeof(mm_header_t);
	}	while (header->free==1 && ptr<=(uint8_t*)mm_end - sizeof(mm_header_t));	
	
	// Run statistics to verify integrity
	struct mm_stat mmstat;
	sys_mmstat(&mmstat);

	if (schedlock)
		sched_unlock();
// 	mm_status();
}


/*#define MEM_POOL_SIZE 1000

int main(int argc, char** argv) {
// 	void *ptr[MEM_POOL_SIZE];
	long size;
	struct timeval t;
	void* mem[MEM_POOL_SIZE];
	int i;
	unsigned int timer = 0;
	unsigned char leftright = 0;
	
	gettimeofday(&t,NULL);
	
	mm_init(memory,sizeof(memory));
	
	srandom(t.tv_usec);
	
	for (i=0; i<MEM_POOL_SIZE; i++)
		mem[i] = 0;
	
	//mm_status();
	while (1) {
		void* ptr;
		
		size = random()%(sizeof(memory)>>3)+1;
		ptr = mm_alloc(size);
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
						mm_free(mem[i]);
						mem[i] = 0;
						break;
					}
				}
			} else {
				for (i=MEM_POOL_SIZE-1 ; i>=0; i--) {
					if (mem[i] != 0) {
// 						printf("Free idx %d\n",i);
						mm_free(mem[i]);
						mem[i] = 0;
						break;
					}
				}
			}
		}
		
		if (timer == 100000) {
			mm_status();
// 			usleep(100000);
			timer = 0;
		} else
			timer++;
	}
	
	return 0;
}*/
