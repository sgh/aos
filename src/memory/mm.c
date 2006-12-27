// #include <stdio.h>
#include <kernel.h>
#include <string.h>
#include <driver_core.h>
#include <bits.h>


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
static uint8_t* mm_ptr;

typedef struct mm_header mm_header_t;
struct mm_header {
	/*unsigned char free:1;
	unsigned short size:15;*/
	uint8_t free:1;
	uint8_t padding:2;
	uint16_t size:13;
};

void mm_status(void) {
	int segment = 0;
	uint8_t* ptr = (unsigned char*)mm_start;
	mm_header_t* header;
	uint32_t total_size = 0;
// 	int i;

	do {
		header = (mm_header_t*)ptr;
		total_size += header->size + sizeof(mm_header_t);
// 		printf("Segment %d: %4d bytes (%s)\n",segment,header->size,header->free?"F":"U");
		ptr = ptr + header->size + sizeof(mm_header_t);
// 		if (segment > largest_segmentnum)
// 			largest_segmentnum = segment;
		segment++;
	} while (ptr<mm_end);
// 	printf("Total size : %4d\n",total_size);
// 	printf("Largest segmentnum: %d\n", largest_segmentnum);
// 	assert(total_size == memory_size);
// 	printf("\n");
}

void aos_mm_init(void* start, void* end) {
	struct mm_header* head;
	mm_start =  start;
	mm_end = end;
	memset(mm_start, 0x0, end-start);
// 	memory_size = len;

	head = (struct mm_header*)mm_start;
	head->free = 1;
	head->size = (end-start) - sizeof(mm_header_t);
// 	mm_status();
}

void* sys_malloc(uint16_t size)
{
	uint32_t segmentsize;
	uint8_t* ptr = (uint8_t*)mm_start;
	mm_header_t* header;
	mm_header_t* next_header;

//  	mm_status();
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
			header->free=0;
			header->size=size;
			
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
			return ptr + sizeof(mm_header_t);
		}
		ptr += header->size + sizeof(mm_header_t);
	} while ((uint8_t*)ptr<mm_end);
// 	printf("Not enough memory\n");
// 	mm_status();
	
	if (condition_handlers && condition_handlers->oom)
		condition_handlers->oom(current); // Call condition-handler
	
	return NULL; // Out-Of-Memory
}

void sys_free(void* segment) {
	mm_header_t* header =  segment - sizeof(mm_header_t);
	uint8_t *ptr = (uint8_t*)header;
	mm_header_t* prev_header = NULL;

// 	mm_status();
	
	header->free=1;
	do {
		header = (mm_header_t*)ptr;
		if (prev_header && header->free==1 && prev_header->free==1) {
			prev_header->size += header->size + sizeof(mm_header_t);
		}	else
			prev_header = header;
		ptr += header->size + sizeof(mm_header_t);
	}	while (header->free==1 && ptr<=(uint8_t*)mm_end - sizeof(mm_header_t));	
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
