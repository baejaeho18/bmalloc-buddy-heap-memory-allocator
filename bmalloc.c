#include <unistd.h>
#include <stdio.h>
#include "bmalloc.h" 

bm_option bm_mode = BestFit ;
bm_header bm_list_head = {0, 0, 0x0 } ;

void * sibling (void * h)
{
	// TODO : returns the header address of the suspected sibling block of h
	
}

int fitting (size_t s) 
{
	// TODO : return the size field value of a fitting blocks to accommodate s bytes
	int size;
	for(size = 16 ; size <= 4096 ; size *= 2){
		if(s + 9 < size){
			// print("%d", size) ;
			return size ;
		}
	}
	size = ((s + 9) % 4096) * 4096 + 4096 ;
	// print("%d", size) ;
	return size ;
}

void * bmalloc (size_t s) 
{
	// mmap()
	char *p = NULL ;
	int fitting_size = fitting(s) ;
	bm_header_ptr itr ;
	// TODO : allocate a buffer of s-bytes and returns its starting address
	// find fitting block
	if(bm_option == 0){	// Bestfit

	}
	else{	// bm_option == 1 : Firstfit
		for (itr = bm_list_head.next ; itr != 0x0 ; itr = itr->next) {
			if((int)itr->size >= fitting_size){
				break ;
			}
		}
		
	}
	// 할당하고
	if((int)itr->size == fitting_size){
		itr->used = 1 ;
		return itr ;
	}
	// 자르고
	else{
		while(!(int)itr->size == fitting_size){
			char *tmp_next = itr->next ;
			int tmp_size = itr->size ;
			
			itr->size = tmp_size / 2 ;
			itr->next = itr + itr->size ;
			
			itr->next->size = tmp_size / 2 ;
			itr->next->next = tmp_next ;
		}

	}
	// 헤더바꾸고

	// if no available fitting block
	if((p = (char *) mmap(0, 4096,  PROT_WRITE | PROT_READ, MAP_SHARED, -1, 9)) == -1){
		perror("mmap error\n") ;
		exit(1) ;
	}
	


	return p ;
}

void bfree (void * p) 
{
	// TODO : free the allocated buffer srarting at pointer p
	// before header -> next header
	// header.used->1
	// merge with sibiling
	// if 4096, unmap()
	if (munmap(p, 4096) == -1){
		perror("munmap error\n") ;
		exit(1) ;
	};
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
