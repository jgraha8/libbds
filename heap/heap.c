#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "sysutil.h"

#define SETBIT( a, mask ) ( (a) |= (mask) )
#define CLRBIT( a, mask ) ( (a) &= ( (a) ^ (mask) ) )
#define TOGBIT( a, mask ) ( (a) ^= (mask) )

#define HEAP_PARENT(i) ( ((i+1)>>1) - 1 )
#define HEAP_LEFT(i) ( ((i+1)<<1) - 1 )
#define HEAP_RIGHT(i) ( (i+1)<<1 )

#define HEAP_TYPE_MIN -1
#define HEAP_TYPE_MAX 1

// flags: [00000][sorted][heaptype][isheap]

#define HEAP_ISHEAP   1
#define HEAP_TYPE_MIN 0<<1
#define HEAP_TYPE_MAX 1<<1
#define HEAP_SORTED   1<<2

#define SET_BIT(a,mask) 

// flags : isheap, heaptype, sorted

typedef struct {
	int elem_size;
	int max_len;
	int len;
	int heap_len;
	unsigned char flags;
	void *data; // Managed internally
	int (*compar)( const void *, const void *);
	void (*heapify)( ds_heap_t *, int );
	void (*heap_adjust)( ds_heap_t *, int );
	void (*increase_key)( ds_heap_t *, int, const void *);
	void (*build)( ds_heap_t * );
	void (*insert)( ds_heap_t *, const void * );	
} ds_heap_t;

static inline void *heap_ptr( ds_heap_t *h, int i )
{
	assert( 0 <= i && i < h->len );
	return h->data + i*h->elem_size;
}

static inline void heap_memswap( ds_heap_t *h, int i, int j )
{
	memswap( heap_ptr(h,i), heap_ptr(h,j), h->elem_size );
}

static inline void heap_memcpy( ds_heap_t *h, int i, int j )
{
	memcpy( heap_ptr( h, i ), heap_ptr( h, j ), h->elem_size );
}

static void __max_heapify( ds_heap_t *h, int i )
{
	int l = HEAP_LEFT(i);
	int r = HEAP_RIGHT(i);
	int largest = 0;
	
	if( l < h->heap_len && h->compar( heap_ptr(h,l), heap_ptr(h,i) ) > 0 ) {
		largest = l;
	} else {
		largest = i_;
	}
	if( r < h->heap_len && h->compar( heap_ptr(h,r), heap_ptr(h,largest) ) > 0 ) {
		largest = r;
 	}

	if( largest != i ) {
		heap_memswap( h, i, largest );
		heap_maxheapify( h, largest );
	}
	
}

static void __max_heap_adjust( ds_heap_t *h, int i )
{
	int p = HEAP_PARENT(i);
	while( i > 0 && h->compar( heap_ptr(h,p), heap_ptr(h,i) ) < 0 ) {
		heap_memswap( h, i, p );
		i = p;
		p = HEAP_PARENT(i);
	}
}


static void __max_increase_key( ds_heap_t *h, int i, const void *key )
{
	if( h->compar( key, heap_ptr(h,i) ) < 0 ) {
		fprintf(stderr, "new key is smaller than current key\n");
		exit(EXIT_FAILURE);
	}

	memcpy( heap_ptr(h,i), key, h->elem_size );
	heap_adjust_maxheap( h, i );
}

static void __max_build( ds_heap_t *h )
{
	h->heap_len = h->len;

	if( h->flags & ( HEAP_ISHEAP | HEAP_TYPE_MAX ) )
		return;
	
	SETBIT( h->flags, HEAP_ISHEAP | HEAP_TYPE_MAX );
	CLRBIT( h->flags, HEAP_SORTED );
	
	int i;
	for( i=HEAP_PARENT(h->heap_len - 1 ); i>=0; --i ) {
		h->heapify( h, i );
	}
}

void __max_insert( ds_heap_t *h, const void *key )
{
	h->heap_len++;
	h->len++;

	if( h->heap_len > h->max_len ) {
		size_t size = h->max_len * h->elem_size;
		h->data = xrealloc( h->data, size, 2*size );
		h->max_len *= 2;
	}

	memcpy( heap_ptr(h, h->heap_len-1), key, h->elem_size );
	h->adjust( h, h->heap_len - 1 );
}

static inline void *ds_heap_data( ds_heap_t *h )
{
	return h->data;
}

static inline const void *ds_heap_data_const( const ds_heap_t *h )
{
	return (const void *)h->data;
}

void ds_heap_ctor( ds_heap_t *h, int elem_size, int len, enum heap_type type,
		   int (*compar)(const void *, const void *) )
{
	memset( h, 0, sizeof(*h) );

	h->elem_size = elem_size;
	h->len = len;
	h->max_len = len;
	h->data = xalloc( elem_size*len );

	switch( type ) {
	case MAX_HEAP:
		h->heapify      = __max_heapify;
		h->adjust       = __max_heap_adjust;
		h->increase_key = __max_increase_key;
		h->build        = __max_build;
		h->insert       = __max_insert;
	case MIN_HEAP:
		h->heapify      = __min_heapify;
		h->adjust       = __min_heap_adjust;
		h->increase_key = __min_increase_key;
		h->build        = __min_build;
		h->insert       = __min_insert;
	default:
		fprintf(stderr, "incorrect heap type\n");
		exit(EXIT_FAILURE);
	}
	
	h->compar = compar;
}

void ds_heap_dtor( ds_heap_t *h )
{
	if( h->data ) free( h->data );
	
	memset( h, 0, sizeof(*h) );
}

static inline void ds_heap_insert( ds_heap_t *h, const void *key )
{
	h->insert( h, key );
}

static inline void ds_heap_build( ds_heap_t *h )
{
	h->build(h);
}

void ds_heap_sort( ds_heap_t *h )
{
	int i;

	h->build( h );

	for( i = h->len-1; i>=1; --i ) {
		heap_memswap( h, 0, i ); // Place the ith value at root
		h->heap_len--;
		h->heapify( h, 0 );
	}

	CLRBIT( h->flags, HEAP_ISHEAP );
	SETBIT( h->flags, HEAP_SORTED );
}



int ds_heap_get( ds_heap_t *h, void *val )
{
	memcpy(val, h->data, h->elem_size );
	return 0;
}

int ds_heap_pop( ds_heap_t *h, void *val )
{
	memcpy(val, h->data, h->elem_size );

	// Place the last value at root
	heap_memcpy( h, 0, h->heap_len-1 );
	h->heap_len--;

	h->heapify( h, 0 );
	
	return 0;
}

int compar_double( const void *a_, const void *b_ )
{
	if( *((double *)a_) < *((double *)b_) ) return -1;
	if( *((double *)a_) > *((double *)b_) ) return 1;
	return 0;
}

int compar_int( const void *a_, const void *b_ )
{
	if( *((int *)a_) < *((int *)b_) ) return -1;
	if( *((int *)a_) > *((int *)b_) ) return 1;
	return 0;
}

struct junk {
	int key;
	double val;
};

int compar_junkkey( const void *a_, const void *b_ )
{
	if( ((struct junk *)a_)->key < ((struct junk *)b_)->key ) return -1;
	if( ((struct junk *)a_)->key > ((struct junk *)b_)->key ) return 1;
	return 0;
}
int compar_junkval( const void *a_, const void *b_ )
{
	if( ((struct junk *)a_)->val < ((struct junk *)b_)->val ) return -1;
	if( ((struct junk *)a_)->val > ((struct junk *)b_)->val ) return 1;
	return 0;
}

int main(void)
{
#define N 9

	int i,j;
	
	double *A;

	ds_heap_t heap;
	ds_heap_ctor( &heap, sizeof(double), N, compar_double );

	A = ds_heap_data( &heap );

	for( i=0; i<N; ++i ) {
		j = N-1-i;
		A[i] = j;
		printf("A[%d] = %15.7e\n", i, A[i] );
	}
	printf("================================\n");
	
	ds_heap_build( &heap );
	printf("maxbuild: +++++++++++++++++++\n");
	for( i=0; i<N; ++i ) {
		printf("A[%d] = %15.7e\n", i, A[i] );
	}
	printf("+++++++++++++++++++\n");


	double key;

	key = 1.687;
	ds_heap_insert( &heap, &key );
	A = ds_heap_data( &heap );
	
	printf("maxinsert: +++++++++++++++++++\n");
	printf("heap.len = %d\n", heap.len);
	for( i=0; i<heap.len; ++i ) {
		printf("A[%d] = %15.7e\n", i, A[i] );
	}
	printf("+++++++++++++++++++\n");
	
	ds_heap_sort( &heap );
	
	for( i=0; i<heap.len; ++i ) {
		printf("A[%d] = %15.7e\n", i, A[i] );
	}
	
	ds_heap_dtor( &heap );
}
