#include <sys/time.h>
#include <stdio.h>

// #define NULL 0

/* The memory we allocate from */
char __attribute__((aligned(4))) memory[30];


/* MM defines */
#define FREE_BIT 0x8000
#define SIZE_BIT 0x7FFF
#define UPPER_BYTE 0xFF00
#define LOWER_BYTE 0x00FF


/* MM status variables */
unsigned char* mm_start;
unsigned char* mm_end;
unsigned char* mm_ptr;

typedef struct mm_header mm_header_t;
struct mm_header {
	unsigned char free:1;
	unsigned short size:15;
};

void mm_status(void) {
	int segment = 0;
	unsigned char* ptr = (unsigned char*)mm_start;
	mm_header_t* header;
	unsigned int total_size = 0;
	int i;
	
	printf("\n");
	for (i=0; i<sizeof(memory); i++) {
		printf ("0x%.2X ",memory[i]);
	}
	printf("\n");
	
	do {
		header = (mm_header_t*)ptr;
		total_size += header->size + sizeof(mm_header_t);
		printf("Segment %d: %4d bytes (%s)\n",segment,header->size,header->free?"F":"U");
		ptr = ptr + header->size + sizeof(mm_header_t);
		segment++;
	} while (ptr<mm_end);
	printf("Total size : %4d\n",total_size);
	printf("\n");
}

void mm_init(void* start, unsigned short len) {
	struct mm_header* head;
	mm_start =  start;
	mm_end = mm_start + len;	
	memset(mm_start,0x0,len);

	head = (struct mm_header*)mm_start;
	head->free=1;
	head->size=len-sizeof(mm_header_t);
	mm_status();
}

void* mm_alloc(unsigned short size)
{
	unsigned int segmentsize;
	unsigned char* ptr = (unsigned char*)mm_start;
	mm_header_t* header;
	mm_header_t* next_header;
	
	do {
		header = (mm_header_t*)ptr;
		next_header = (mm_header_t*)(ptr + header->size + sizeof(mm_header_t));
		if (header->free==1 && (unsigned char*)next_header<mm_end && next_header->free==1) {
			header->size += next_header->size + sizeof(mm_header_t);
		}
		if (header->size>=size && header->free==1) {
			segmentsize = header->size;
			printf("Allocating %d bytes on offset %d\n",size,(unsigned char*)ptr-mm_start+sizeof(mm_header_t));
			header->free=0;
			header->size=size;
			header = (mm_header_t*) (ptr + sizeof(mm_header_t) + size);
			if ((unsigned char*)header<mm_end) {
				header->free=1;
				header->size = segmentsize - size - sizeof(mm_header_t);
			}
			mm_status();
			return ptr + sizeof(mm_header_t);
		}
		ptr += header->size + sizeof(mm_header_t);
	} while ((unsigned char*)ptr<mm_end);
	printf("Not enough memory\n");
	return NULL;
}

void mm_free(void* segment) {
	mm_header_t* header =  segment - sizeof(mm_header_t);
	unsigned char *ptr = (unsigned char*)header;
	mm_header_t* prev_header = NULL;
	
	header->free=1;
	do {
		header = (mm_header_t*)ptr;
		if (prev_header && header->free==1 && prev_header->free==1) {
			prev_header->size+= header->size + sizeof(mm_header_t);
		}	else
			prev_header = header;
		ptr += header->size + sizeof(mm_header_t);
	}	while (header->free==1 && ptr<=(unsigned char*)mm_end - sizeof(mm_header_t));	
	mm_status();
}



int main(int argc, char** argv) {
	void *ptr[10];
	long size;
	struct timeval t;
	void* mem[10];
	int i;
	
	gettimeofday(&t,NULL);
	
	mm_init(memory,sizeof(memory));
	
	srandom(t.tv_usec);
	
	for (i=0; i<10; i++)
		mem[i] = 0;
	
	mm_status();
	while (1)
	{
		void* ptr;
		
		size = random()%(sizeof(memory)>>2)+1;
		ptr = mm_alloc(size);
		if (ptr) { // If alloc succeded place it in the alloc-pool
			printf("Alloc %d\n",size);
			for (i=0 ; i<10; i++) {
				if (mem[i] == 0) {
					mem[i] = ptr;
					break;
				}
			}
		} else { // Free a segment if alloc failed
			for (i=0 ; i<10; i++) {
				if (mem[i] != 0) {
					printf("Free idx %d\n",i);
					mm_free(mem[i]);
					mem[i] = 0;
					break;
				}
			}
		}
		
		mm_status();
		usleep(500000);
	}
	
	return 0;
}
