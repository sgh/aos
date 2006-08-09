#include <fragment.h>
// #include <string.h>
#include <string.h>
#include <driver_core.h>

#define ciel(var,val) (var>val?val:var)

struct fragment_store storage[50];

struct fragment_store* free_list;



struct fragment_store* store_fragment(const unsigned char* data, unsigned int size) {
	struct fragment_store* retval = free_list;
	struct fragment_store* prevfrag = 0;
	
//  	printf("storing: ");
	while (size>0 && free_list) {
		free_list->size = ciel(size,FRAGMENT_SIZE);
		size -= free_list->size;
// 		printf("%d ",free_list->size);
		memcpy(free_list->data,data,free_list->size);
		data += free_list->size;
		prevfrag = free_list;
		free_list = free_list->next;
	}
//  	printf("\n");
	if (retval && prevfrag) prevfrag->next = 0;
	return retval;
}

void load_fragment(unsigned char* data, struct fragment_store* fragment) {
	struct fragment_store* prevfrag;

// 	printf("loading: ");
	while (fragment) {
// 		printf(" %d",fragment->size);
		memcpy(data,fragment->data,fragment->size);
		data += fragment->size;
		prevfrag = fragment;
		fragment = fragment->next;
		
		/* Free prevfragment */
		prevfrag->next = free_list;
		free_list = prevfrag;
	}
// 	printf("\n");
}

void  init_fragment_store() {
	int i;
	int num_fragments = sizeof(storage)/sizeof(storage[0]);
	for (i=1; i<num_fragments; i++) {
		storage[i-1].next = &storage[i];
		storage[i].next = 0;
	}
	free_list = &storage[0];
}

unsigned int free_fragments() {
	struct fragment_store* fragptr = free_list;
	unsigned int count = 0;
	while (fragptr) {
		count++;
		fragptr = fragptr->next;
	}
	return count;
}

/*
int main() {
	struct fragment_store* frag1;
	struct fragment_store* frag2;
	unsigned char arr1[200] = "array 1  1 array 1  1 array 1  1 array 1 1 1 1 \0";
	unsigned char arr2[200] = "array 2  2 array 2  2 array 2  2 array 2 2 2 2 \0";
	init_fragment_store();
	
	printf("Free fragments: %d\n",free_fragments());
	
	frag1 = store_fragment(arr1,sizeof(arr1));
	frag2 = store_fragment(arr2,sizeof(arr2));
	
	memset(arr1,0,sizeof(arr1));
	memset(arr2,0,sizeof(arr2));
	
	printf("Free fragments: %d\n",free_fragments());
	
	load_fragment(arr1,frag1);
	load_fragment(arr2,frag2);
	
	printf("Free fragments: %d\n",free_fragments());
	
	printf("%s\n",arr1);
	printf("%s\n",arr2);
	
}

*/
