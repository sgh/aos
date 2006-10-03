#include <types.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define DEBUG

struct bipsegment {
	uint16_t idx;
	uint16_t size;
};

struct bipbuffer {
	uint8_t* alloc;
	uint16_t size;
	uint16_t reserved;
	uint8_t segment;
	struct bipsegment segment_a;
	struct bipsegment segment_b;
};



/**
 * \brief Initialize the bipbuffer
 * @param bip 
 * @param alloc 
 * @param size 
 */
void bipbuffer_init(struct bipbuffer* bip, uint8_t* alloc, uint16_t size) {
	memset(alloc, 0, size);
	bip->alloc = alloc;
	bip->size = size;
}


/**
 * \brief Reserve a piece of memory from the buffer 
 * @param bip 
 * @param size the amount of memory the want to reserve
 * @param reserved pointer to variable to store the size of memory actualy reserved
 * @return pointer to the memory reserved
 */
char* bipbuffer_reserve(struct bipbuffer* bip, uint16_t size, uint16_t* reserved) {
	struct bipsegment* segment;
	
	
	if (bip->segment_b.size == 0) { // If segment B is not in use
		uint16_t leftfree;
		uint16_t rightfree;

		// Calculate freespace on left and right size of segment A
		leftfree = bip->segment_a.idx;
		rightfree = bip->size - bip->segment_a.idx - bip->segment_a.size;
		
// 		printf("leftfree : %d\n", leftfree);
// 		printf("rightfree : %d\n", rightfree);

		// If the space on the left size of the segment is largest, use next segment
		if (leftfree > rightfree && bip->segment_b.size == 0) { // TODO allocated from right if the requested size fits within free space
#ifdef DEBUG
			printf("USE NEW SEGMENT\n");
#endif
			bip->reserved = leftfree;
			segment = &bip->segment_b;
			bip->segment_b.idx = 0;
			bip->segment_b.size = 0;
			bip->segment = 1;
		} else {
#ifdef DEBUG
			printf("USE SEGMENT A\n");
#endif
			bip->reserved = rightfree;
			segment = &bip->segment_a;
			bip->segment = 0;
		}

	} else {
#ifdef DEBUG
		printf("USE SEGMENT B\n");
#endif
		bip->reserved = bip->segment_a.idx - (bip->segment_b.idx + bip->segment_b.size);
		segment = &bip->segment_b;
		bip->segment = 1;
	}

	*reserved = bip->reserved;

#ifdef DEBUG
	printf("reserved : %d\n", *reserved);
#endif
	return bip->alloc + segment->idx + segment->size;
}

/**
 * \brief Commit the former reserved memory
 * @param bip 
 * @param size the size to commit
 */
void bipbuffer_commit(struct bipbuffer* bip, uint16_t size) {
	assert(bip->reserved >= size);

	if (bip->segment == 0)
		bip->segment_a.size += size;
	else
		bip->segment_b.size += size;

	bip->reserved = 0;
#ifdef DEBUG
	printf("commit : %d [%d]\n",size, bip->segment);
#endif
}


/**
 * \brief Get the first contigous block
 * @param bip 
 * @param size
 * @return Pointer to the block
 */
char* bipbuffer_getblock(struct bipbuffer* bip, uint16_t* size) {

	*size = bip->segment_a.size;

// 	printf("getblock %d\n", *size);
			
	if (*size == 0)
		return (void*)0;
	return bip->alloc + bip->segment_a.idx;
}

/**
 * \brief Consume data from the buffer
 * @param bip 
 * @param size 
 */
void bipbuffer_decommit(struct bipbuffer* bip, uint16_t size) {
	assert(size <= bip->segment_a.size);

	bip->segment_a.idx += size;
	bip->segment_a.size -= size;

	if (bip->segment_a.size == 0) {
// 		printf("segment_a empty\n");
		memcpy(&bip->segment_a, &bip->segment_b, sizeof(struct bipsegment));
		memset(&bip->segment_b, 0, sizeof(struct bipsegment));
	}
#ifdef DEBUG
	printf("decommit %d\n", size);
#endif
}

void bipbuffer_status(struct bipbuffer* bip) {
#ifdef DEBUG
	printf("A: %d@%d   B: %d@%d\n", bip->segment_a.size, bip->segment_a.idx, bip->segment_b.size, bip->segment_b.idx);
#endif
}


struct bipbuffer biptest;
char testbuffer[64];

void output(char* ptr, int size) {
// 	printf("::: DATA: ");
	while (size--) {
		printf("%c", *ptr);
		ptr++;
	}
// 	printf("\n");
}

int main() {
	uint16_t reserved;
	char* ptr;
	uint16_t size;
	int i;
	int checkint = 0;
	int gint = 0;
	char* str = "hello worldhello worldhello worldhello world";
	char* smallstr = "abcdefghijkl";

	bipbuffer_init(&biptest, testbuffer, sizeof(testbuffer));
	
	while (gint < 300) {
		int commitsize;
		uint16_t reserved;
		ptr = bipbuffer_reserve(&biptest,5,&reserved);
		assert(ptr <= &testbuffer[64-reserved]);
		if (ptr) {
			commitsize = snprintf(ptr, reserved,"%d\n", gint);
			if (reserved >= commitsize) {
				bipbuffer_status(&biptest);
#ifdef DEBUG
				printf("commit: %d\n",gint);
#endif
				bipbuffer_commit(&biptest, commitsize);
				bipbuffer_status(&biptest);
				gint++;
			}
		} else
			printf("BUFFER FULL\n");

		bipbuffer_status(&biptest);

		if (random() > RAND_MAX>>1)
		if (ptr = bipbuffer_getblock(&biptest, &size)) {
			int decommitsize = random() % size + 1;
			output(ptr,decommitsize);
			bipbuffer_decommit(&biptest, decommitsize);
		}
// 		usleep(100000);
		
	}

	/*ptr = bipbuffer_reserve(&biptest,0,&reserved);
	memcpy(ptr, str, strlen(str));
	bipbuffer_commit(&biptest, strlen(str));
	while (ptr = bipbuffer_getblock(&biptest, &size)) {
// 		printf("PTR: %u\n", ptr);
		output(ptr,size);
		bipbuffer_decommit(&biptest, size);
}*/
}
