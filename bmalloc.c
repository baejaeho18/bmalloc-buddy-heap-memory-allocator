#include <unistd.h>
#include <stdio.h>
#include "bmalloc.h" 

bm_option bm_mode = BestFit ;
bm_header bm_list_head = {0, 0, 0x0 } ;	// next points the first header of pages

void * sibling (void * h)
{
	// TODO : returns the header address of the suspected sibling block of h
	int size = (bm_header *)h->size ;

	bm_header_ptr itr ;
	bm_header_ptr pre ;
	int piled_size = 0 ;

	for (itr = bm_list_head.next ; itr != 0x0 || itr != (char *)h ; itr = itr->next) {
		piled_size += itr->size ;
		pre = itr ;
	}
	if (itr == 0x0) {
		perror("wrong header pointer\n") ;
	}
	else if ((piled_size % size) % 2 == 1) { // right side
		return (char *)h->next ; 
	}
	else {	// left side
		return pre->next ;
	}
}

int fitting (size_t s) 
{
	// TODO : return the size field value of a fitting blocks to accommodate s bytes
	int size;
	for (size = 16 ; size <= 4096 ; size *= 2) {
		if (s + 9 < size) {
			// print("%d", size) ;
			return size ;
		}
	}
	size = ((s + 9) % 4096) * 4096 + 4096 ;
	print("How do I manage the size:%d?\n", size) ;
	return size ;
}

void * bmalloc (size_t s) 
{
	// TODO : allocate a buffer of s-bytes and returns its starting address
	int fitting_size = fitting(s) ;
	bm_header_ptr itr ;
	bm_header bm_list_tail = bm_list_head ;
	// find fitting block
	if (bm_option == 0) {	// Bestfit
		bm_header_ptr bt = 0x0;
		for (itr = bm_list_head.next ; itr != 0x0 ; itr = itr->next) {
			if (fitting_size <= (int)itr->size && (int)itr->size < bt->size) {
				bt = itr ;
			}
			bm_list_tail = itr ; 
		}
		itr = bt ;
	}
	else {	// bm_option == 1 : Firstfit
		for (itr = bm_list_head.next ; itr != 0x0 ; itr = itr->next) {
			if (fitting_size <= (int)itr->size) {
				break ;
			}
			bm_list_tail = itr ;
		}
	}
	// if no available fitting block
	if (itr == 0x0) {	// itr->size < fitting_size
		if ((itr = (char *) mmap(0, 4096,  PROT_WRITE | PROT_READ, MAP_SHARED, -1, 9)) == -1) {
			perror("mmap error\n") ;
			exit(1) ;
		}
		bm_list_tail->next = itr ;
		itr->used = 0 ;
		itr->size = 4096 ;
		itr->next = 0x0 ;
		return bmalloc(s) ;
	}
	// 할당하고
	else if ((int)itr->size == fitting_size) {
		itr->used = 1 ;
		return itr ;
	}
	// 자르고
	else if ((int)itr->size > fitting_size) {
		while (!(int)itr->size == fitting_size) {
			char *tmp_next = itr->next ;
			int tmp_size = itr->size ;

			itr->size = tmp_size / 2 ;
			itr->next = itr + tmp_size / 2 ;
			itr->next->size = tmp_size / 2 ;
			itr->next->next = tmp_next ;

			itr = itr->next;
		}
		itr->used = 1 ;
		return itr ;
	}
	else {	// itr->size < fitting_size
		perror("Impossible happens, you babarian") ;
		exit(1) ;
	}
}

void bfree (void * p) 
{
	// TODO : free the allocated buffer srarting at pointer p
	// before header -> next header & header.used->1
	(char *) p->used = 0 ;
	
	// merge with sibiling
	// if 4096, unmap()
	if (munmap(p, 4096) == -1) {
		perror("munmap error\n") ;
		exit(1) ;
	};
}

void * brealloc (void * p, size_t s) 
{
	// TODO : resize the allocated memory buffer into s bytes
	if ((char*)p->size < fitting(s)) {
		// maintain p and slice the size? or just free and alloc?
		bfree(p) ;
		return bmalloc(s) ;
	}
	else if ((char*)p == fitting(s)) {
		return p ;
	}
	else {	// p.size > s
		bfree(p) ;
		return bmalloc(s) ;
	}
}

void bmconfig (bm_option opt) 
{
	// TODO : set as BestFit or FirstFit
	bm_mode = opt;
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
	// total size, total used size, total available size, total internal fragment
	// each block's used, size, payload size
}
