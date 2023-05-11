#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdio.h>
#include "bmalloc.h" 

bm_option bm_mode = BestFit ;
bm_header bm_list_head = {0, 0, 0x0 } ;	// next points the first header of pages
bm_header_ptr pre_siblings ;
bm_header_ptr post_siblings ;
int total_size = 0 ;

void * sibling (void * h)
{
	// TODO : returns the header address of the suspected sibling block of h
	int size = ((bm_header *)h)->size ;

	bm_header_ptr itr ;
	bm_header_ptr pre = 0x0 ;
	int piled_size = 1 ;	// why?

	for (itr = bm_list_head.next ; itr != 0x0 || itr != (bm_header *)h ; itr = itr->next) {
		piled_size += (1 << itr->size) ;
		pre_siblings = pre ;
		pre = itr ;
	}
	if (itr == 0x0) {
		perror("wrong header pointer\n") ;
	}
	else if ((piled_size / size) % 2 == 1) { // left side
		pre_siblings = pre ;
		post_siblings = ((bm_header *)h)->next->next ;
		return ((bm_header *)h)->next ; 
	}
	else {	// right side
		post_siblings = ((bm_header *)h)->next ;
		return pre ;
	}
}

int fitting (size_t s) 
{
	// TODO : return the size field value of a fitting blocks to accommodate s bytes
	for (int size = 16, val = 4 ; size <= 4096 ; size *= 2, val++) {
		if ((int)s + 9 < size) {
			return val ;
		}
	}
	// size = ((s + 9) % 4096) * 4096 + 4096 ;
	// printf("How do I manage the size:%d?\n", size) ;
	return -1 ;
}

void * bmalloc (size_t s) 
{
	bm_header_ptr itr ;
	bm_header_ptr bm_list_tail ;
	int fitting_size = fitting(s) ;

	if (total_size == 0) {
		if ((itr = mmap(NULL, 4096,  PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS , -1, 0)) == MAP_FAILED) {
			perror("mmap error\n") ;
			exit(1) ;
		}
		printf("1 %d, %d, %p\n", itr->used, itr->size, itr->next) ;
		total_size += 4096 ;
		bm_list_head.next = itr ;
		itr->used = 0 ;
		itr->size = 12 ;
		itr->next = 0x0 ;
	}

	bm_header_ptr fitting_block = malloc(sizeof(bm_header)) ;
	fitting_block->size = 13 ;
	for (itr = bm_list_head.next ; itr != 0x0 ; itr = itr->next) {
		if (fitting_size <= itr->size && itr->used == 0) {
			if (bm_mode == FirstFit) {
				fitting_block = itr ;
				break ;
			}
			else /* Bestfit */ if (itr->size < fitting_block->size) {
				fitting_block = itr ;
			}
		}
		if (itr->next == 0x0) {
			bm_list_tail = itr ;
		}
	}

	if (12 < fitting_block->size) {
		// no fitting block : mmap
		if ((itr = mmap(NULL, 4096,  PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS , -1, 0)) == MAP_FAILED) {
			perror("mmap error\n") ;
			exit(1) ;
		}
		total_size += 4096 ;
		bm_list_tail->next = itr ;
		itr->used = 0 ;
		itr->size = 12 ;
		itr->next = 0x0 ;
		fitting_block = itr ;
	}
	for ( ; fitting_size < fitting_block->size ; fitting_block->size = fitting_block->size - 1) {
		bm_header_ptr fitting_next = fitting_block->next ;

		fitting_block->next = (bm_header_ptr) ((char *)fitting_block + (1 << (fitting_block->size - 1))) ;
		fitting_block->next->used = 0 ;
		fitting_block->next->size = fitting_block->size - 1;
		fitting_block->next->next = fitting_next ;

	}
	fitting_block->used = 1 ;
	return fitting_block ;
}

void bfree (void * p) 
{
	// TODO : free the allocated buffer srarting at pointer p
	((bm_header *)p)->used = 0 ;

	bm_header_ptr itr ;
	for (itr = p ; itr->size == ((bm_header *)sibling(itr))->size ; itr = pre_siblings->next) {
		pre_siblings->next == post_siblings ;
	}

	if (itr->size == 4096) {
		if (munmap(p, 4096) == -1) {
			perror("munmap error\n") ;
			exit(1) ;
		};
	}
}

void * brealloc (void * p, size_t s) 
{
	bfree(p) ;
	return bmalloc(s) ;
}

void bmconfig (bm_option opt) 
{
	// TODO : set as BestFit or FirstFit
	bm_mode = opt ;
}

void 
bmprint () 
{
	bm_header_ptr itr ;
	int i ;
	int total_used_size = 0;
	int total_available_size = 0;
	int total_internal_fragment = 0;

	printf("==================== bm_list ====================\n") ;
	for (itr = bm_list_head.next, i = 0 ; itr != 0x0 ; itr = itr->next, i++) {
		printf("%3d:%p:%1d %8d:", i, ((void *) itr) + sizeof(bm_header), (int)itr->used, (int) itr->size) ;

		int j ;
		char * s = ((char *) itr) + sizeof(bm_header) ;
		for (j = 0 ; j < (itr->size >= 8 ? 8 : itr->size) ; j++) 
			printf("%02x ", s[j]) ;
		printf("\n") ;

		//TODO: print out the stat's.
		// total size, total used size, total available size, total internal fragment
		// each block's used, size, payload size
		if (itr->used == 0) {	// available(unused)
			total_available_size += (1 << (int) itr->size) ;
		}
		else /* used */ {
			total_used_size += (1 << (int) itr->size) ;
		}
	}
	printf("=================================================\n") ;
	printf("Total size = %d\n", total_size) ;
	printf("Total used size = %d\n", total_used_size) ;
	printf("Total available size = %d\n", total_size - total_used_size) ;
	printf("Total internal fragment = %d\n", total_internal_fragment) ; //b problem
	printf("=================================================\n") ;
	printf("\n") ;
}
