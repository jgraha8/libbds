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
	int (*compare)( const void *, const void *);
} ds_heap_t;

static inline void *heap_ptr( ds_heap_t *this_, int i_ )
{
	assert( 0 <= i_ && i_ < this_->len );
	return this_->data + i_*this_->elem_size;
}

static inline void heap_memswap( ds_heap_t *this_, int i_, int j_ )
{
	memswap( heap_ptr(this_,i_), heap_ptr(this_,j_), this_->elem_size );
}

static inline void heap_memcpy( ds_heap_t *this_, int i_, int j_ )
{
	memcpy( heap_ptr( this_, i_ ), heap_ptr( this_, j_ ), this_->elem_size );
}

static void heap_maxheapify( ds_heap_t *this_, int i_ )
{
	int l = HEAP_LEFT(i_);
	int r = HEAP_RIGHT(i_);
	int largest = 0;
	
	if( l < this_->heap_len && this_->compare( heap_ptr(this_,l), heap_ptr(this_,i_) ) > 0 ) {
		largest = l;
	} else {
		largest = i_;
	}
	if( r < this_->heap_len && this_->compare( heap_ptr(this_,r), heap_ptr(this_,largest) ) > 0 ) {
		largest = r;
 	}

	if( largest != i_ ) {
		heap_memswap( this_, i_, largest );
		heap_maxheapify( this_, largest );
	}
	
}

static void heap_adjust_maxheap( ds_heap_t *this_, int i_ )
{
	int p = HEAP_PARENT(i_);
	while( i_ > 0 && this_->compare( heap_ptr(this_,p), heap_ptr(this_,i_) ) < 0 ) {
		heap_memswap( this_, i_, p );
		i_ = p;
		p = HEAP_PARENT(i_);
	}
}

static void heap_increase_maxkey( ds_heap_t *this_, int i_, const void *key_ )
{
	if( this_->compare( key_, heap_ptr(this_,i_) ) < 0 ) {
		fprintf(stderr, "new key is smaller than current key\n");
		exit(EXIT_FAILURE);
	}

	memcpy( heap_ptr(this_,i_), key_, this_->elem_size );
	heap_adjust_maxheap( this_, i_ );
}

static inline void *ds_heap_getdata( ds_heap_t *this_ )
{
	return this_->data;
}

static inline const void *ds_heap_getdata_const( const ds_heap_t *this_ )
{
	return (const void *)this_->data;
}

void ds_heap_ctor( ds_heap_t *this_, int elem_size_, int len_, int (*compare_)(const void *, const void *) )
{
	memset( this_, 0, sizeof(*this_) );

	this_->elem_size = elem_size_;
	this_->len = len_;
	this_->max_len = len_;
	this_->data = xalloc( elem_size_*len_ );
	this_->compare = compare_;
}

void ds_heap_dtor( ds_heap_t *this_ )
{
	if( this_->data ) free( this_->data );
	
	memset( this_, 0, sizeof(*this_) );
}

void ds_heap_maxbuild( ds_heap_t *this_ )
{
	this_->heap_len  = this_->len;

	if( this_->flags & ( HEAP_ISHEAP | HEAP_TYPE_MAX ) )
		return;
	
	SETBIT( this_->flags, HEAP_ISHEAP | HEAP_TYPE_MAX );
	CLRBIT( this_->flags, HEAP_SORTED );
	
	int i;
	for( i=HEAP_PARENT(this_->heap_len - 1 ); i>=0; --i ) {
		heap_maxheapify( this_, i );
	}
}

void ds_heap_sort( ds_heap_t *this_ )
{
	int i;

	ds_heap_maxbuild( this_ );

	for( i = this_->len-1; i>=1; --i ) {
		heap_memswap( this_, 0, i ); // Place the ith value at root
		this_->heap_len--;           
		heap_maxheapify( this_, 0 );
	}

	CLRBIT( this_->flags, HEAP_ISHEAP );
	SETBIT( this_->flags, HEAP_SORTED );
	
}

void ds_heap_maxinsert( ds_heap_t *this_, const void *key_ )
{
	this_->heap_len++;
	this_->len++;

	if( this_->heap_len > this_->max_len ) {
		size_t size = this_->max_len * this_->elem_size;
		this_->data = xrealloc( this_->data, size, 2*size );
		this_->max_len *= 2;
	}

	memcpy( heap_ptr(this_, this_->heap_len-1), key_, this_->elem_size );

	heap_adjust_maxheap( this_, this_->heap_len-1 );
}

int ds_heap_getmax( ds_heap_t *this_, void *max_ )
{
	if( this_->heap_type != HEAP_TYPE_MAX )
		return -1;
	
	memcpy(max_, this_->data, this_->elem_size );
	return 0;
	
}

int ds_heap_popmax( ds_heap_t *this_, void *max_ )
{
	if( this_->heap_type != HEAP_TYPE_MAX )
		return -1;

	memcpy(max_, this_->data, this_->elem_size );

	// Place the last value at root
	heap_memcpy( this_, 0, this_->heap_len-1 );
	this_->heap_len--;

	heap_maxheapify( this_, 0 );
	
	return 0;
}

int ds_heap_min( ds_heap_t *this_, void *min_ )
{
	if( this_->heap_type != HEAP_TYPE_MIN )
		return -1;
	
	memcpy(min_, this_->data, this_->elem_size );
	return 0;
	
}

int compare_double( const void *a_, const void *b_ )
{
	if( *((double *)a_) < *((double *)b_) ) return -1;
	if( *((double *)a_) > *((double *)b_) ) return 1;
	return 0;
}

int compare_int( const void *a_, const void *b_ )
{
	if( *((int *)a_) < *((int *)b_) ) return -1;
	if( *((int *)a_) > *((int *)b_) ) return 1;
	return 0;
}

struct junk {
	int key;
	double val;
};

int compare_junkkey( const void *a_, const void *b_ )
{
	if( ((struct junk *)a_)->key < ((struct junk *)b_)->key ) return -1;
	if( ((struct junk *)a_)->key > ((struct junk *)b_)->key ) return 1;
	return 0;
}
int compare_junkval( const void *a_, const void *b_ )
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
	ds_heap_ctor( &heap, sizeof(double), N, compare_double );

	A = ds_heap_getdata( &heap );

	for( i=0; i<N; ++i ) {
		j = N-1-i;
		A[i] = j;
		printf("A[%d] = %15.7e\n", i, A[i] );
	}
	printf("================================\n");
	

	ds_heap_maxbuild( &heap );
	printf("maxbuild: +++++++++++++++++++\n");
	for( i=0; i<N; ++i ) {
		printf("A[%d] = %15.7e\n", i, A[i] );
	}
	printf("+++++++++++++++++++\n");


	double key;

	key = 1.687;
	ds_heap_maxinsert( &heap, &key );
	A = ds_heap_getdata( &heap );
	
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
