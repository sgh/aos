#include <fragment.h>
#include <string.h>
#include <driver_core.h>
#include <mm.h>
#include <syscalls.h>

#define ciel(var,val) (var>val?val:var)


struct fragment_store* store_fragment(const unsigned char* data, unsigned int size) {
	struct fragment_store* fragment;
	struct fragment_store* retval = NULL;
	struct fragment_store* prevfrag = NULL;
	
	if (size == 0)
		return NULL;
	
//  	printf("storing: ");
	while (size>0) {
		uint16_t fragment_size = ciel(size,FRAGMENT_SIZE);
		
		fragment = (struct fragment_store*)sys_malloc(sizeof(struct fragment_store));
		if (!fragment) // If malloc did not succeded
			break;
		
		if (retval == NULL) { // First time
			retval = fragment;
			fragment->size = size;
		} else {
			prevfrag->next = fragment;
			fragment->size = fragment_size;
		}

		size -= fragment_size;
		memcpy(fragment->data, data, fragment_size);
		data += fragment_size;
		prevfrag = fragment;
	}
	
	if (size > 0) { // This only happenes if we run out of memory
		free_fragment(retval);
		return NULL;
	}

//  	printf("\n");
	if (retval && prevfrag) prevfrag->next = NULL;
	return retval;
}

void load_fragment(unsigned char* data, struct fragment_store* fragment) {
	struct fragment_store* prevfrag;

// 	printf("loading: ");
	while (fragment) {
		uint16_t fragment_size = ciel(fragment->size, FRAGMENT_SIZE);
// 		printf(" %d",fragment->size);
		memcpy(data,fragment->data, fragment_size);
		data += fragment_size;
		prevfrag = fragment;
		fragment = fragment->next;
		
		/* Free prevfragment */
		sys_free(prevfrag);
		prevfrag = NULL;
	}
// 	printf("\n");
}

void free_fragment(struct fragment_store* fragment) {
	struct fragment_store* prevfrag;

	while (fragment) {
		prevfrag = fragment;
		fragment = fragment->next;
		free(prevfrag);
	}
}

uint16_t size_fragment(struct fragment_store* fragment) {
	return fragment->size;
}


// void  init_fragment_store() {
// 	int i;
// 	int num_fragments = sizeof(storage)/sizeof(storage[0]);
	
// 	for (i=1; i<num_fragments; i++)
// 		storage[i-1].next = &storage[i];
// 	storage[num_fragments-1].next = NULL;
// 	free_list = &storage[0];
// }

// unsigned int free_fragments() {
// 	struct fragment_store* fragptr = free_list;
// 	unsigned int count = 0;
// 	while (fragptr) {
// 		count++;
// 		fragptr = fragptr->next;
// 	}
// 	return count;
// }


void mm_init(void* start, uint16_t len);

char __attribute__((aligned(4))) dmem[4*1024];

/*int main() {
	struct fragment_store* frag1;
	struct fragment_store* frag2;
	unsigned char arr1[200] = "array 1  1 array 1  1 array 1  1 array 1 1 1 1 \0";
	unsigned char arr2[200] = "array 2  2 array 2  2 array 2  2 array 2 2 2 2 \0";
// 	init_fragment_store();
	
	mm_init(dmem, sizeof(dmem));
	
	for (int i = 0; i<100000; i++) {
// 		printf("Free fragments: %d\n",free_fragments());
		
		frag1 = store_fragment(arr1,sizeof(arr1));
		frag2 = store_fragment(arr2,sizeof(arr2));
		
		memset(arr1,0,sizeof(arr1));
		memset(arr2,0,sizeof(arr2));
		
// 		printf("Free fragments: %d\n",free_fragments());
		
		load_fragment(arr1,frag1);
		load_fragment(arr2,frag2);
		
// 		printf("Free fragments: %d\n",free_fragments());
		
// // 		printf("%d:: %s\n", i, arr1);
// // 		printf("%d:: %s\n", i, arr2);
	}
	
}*/


// DRIVER_MODULE_INIT(init_fragment_store);
