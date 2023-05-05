#include <unistd.h>
#include <stdio.h>
#include "bmalloc.h" 

bm_option bm_mode = BestFit ;
bm_header bm_list_head = {0, 0, 0x0 } ;

void * sibling (void * h)
{
	// TODO
}

int fitting (size_t s) 
{
	// TODO
	int size;
	for(size = 16 ; size <= 4096 ; size *= 2){
		if(s + 9 < size){
			// print("%d", size);
			return size;
		}
	}
	size = ((s + 9) % 4096) * 4096 + 4096 ;
	// print("%d", size) ;
	return size ;
}

void * bmalloc (size_t s) 
{
	// TODO : allocate a buffer of s-bytes and returns its starting address
	// mmap()
	return 0x0 ; // erase this
}

void bfree (void * p) 
{
	// TODO : free the allocated buffer srarting at pointer p
	// before header -> next header
	// header.used->1
	// merge with sibiling
	// if 4096, unmap()
}

void * brealloc (void * p, size_t s) 
{
	// TODO : resize the allocated memory buffer into s bytes
	// bfree
	// bmalloc
	return 0x0 ; // erase this 
}

void bmconfig (bm_option opt) 
{
	// TODO : set as BestFit or FirstFit
	
}


void 
bmprint () 
{
	bm_header_ptr itr ;
	int i ;

	printf("==================== bm_list ====================\n") ;
	for (itr = bm_list_head.next, i = 0 ; itr != 0x0 ; itr = itr->next, i++) {
		printf("%3d:%p:%1d %8d:", i, ((void *) itr) + sizeof(bm_header), (int)itr->used, (int) itr->size) ;

		int j ;
		char * s = ((char *) itr) + sizeof(bm_header) ;
		for (j = 0 ; j < (itr->size >= 8 ? 8 : itr->size) ; j++) 
			printf("%02x ", s[j]) ;
		printf("\n") ;
	}
	printf("=================================================\n") ;

	//TODO: print out the stat's.
	// used? size, payload size
}
