typedef enum {
	BestFit, FirstFit
} bm_option ;


struct _bm_header {
	unsigned int used : 1 ;	// if 1 : used, 0 : unused
	unsigned int size : 4 ;	// size of block as base is 2(4~12)
	struct _bm_header * next ;
} ;

typedef struct _bm_header 	bm_header ;
typedef struct _bm_header *	bm_header_ptr ;


void * bmalloc (size_t s) ;

void bfree (void * p) ;

void * brealloc (void * p, size_t s) ;

void bmconfig (bm_option opt) ;

void bmprint () ;
