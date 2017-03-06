/** @file
 *  @brief Queue data structure module
 *
 *  @author Jason Graham <jgraha8@gmail.com>
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include <ckunkwurx/cku_queue.h>
#include "memutil.h"


static int modulo( int a, int p )
{
	return a - floorf(((float)a)/p)*p;
}

static long modulol( long a, long p )
{
	return a - floor(((double)a)/p)*p;
}


static void resize_queue( struct cku_queue *queue )
{
	const size_t n_alloc   = queue->n_alloc << 1; // Double the size
	const size_t elem_len = queue->elem_len;
	
	queue->v = xrealloc( queue->v, queue->n_alloc * elem_len, n_alloc * elem_len );

	cku_queue_linearize( queue );
	queue->n_alloc  = n_alloc;
}

/* __inline__ */
/* static void __queue_incr_back( struct cku_queue *queue ) */
/* { */
/* 	queue->back = ( queue->back + 1 ) % queue->n_alloc; */
/* } */

__inline__
static void __queue_incr_front( struct cku_queue *queue )
{
	queue->front = ( queue->front + 1 ) % queue->n_alloc;
}

/* __inline__ */
/* static void __queue_back( struct cku_queue *queue ) */
/* { */
/* 	return ( queue->front + queue->n_elem - 1 ) % queue->n_alloc; */
/* } */


/* __inline__ */
/* static void __queue_end( struct cku_queue *queue ) */
/* { */
/* 	return ( __queue_back( queue ) + 1 ) % queue->n_alloc; */
/* } */

/* __inline__ */
/* static void *__queue_frontptr( struct cku_queue *queue ) */
/* { */
/* 	return queue->v + queue->front * queue->elem_len; */
/* } */

/* __inline__ */
/* static void *__queue_backptr( struct cku_queue *queue ) */
/* { */
/* 	return queue->v + __queue_back( queue ) + queue->elem_len; */
/* } */

/* __inline__ */
/* static void *__queue_endptr( struct cku_queue *queue ) */
/* { */
/* 	return queue->v + __queue_end( queue ) + queue->elem_len; */
/* } */


void cku_queue_ctor( struct cku_queue *queue, size_t n_alloc, size_t elem_len )
{
	memset(queue, 0, sizeof(*queue));

	queue->n_alloc   = n_alloc;
	queue->elem_len = elem_len;

	queue->v = xalloc( n_alloc * elem_len );
}

void cku_queue_dtor( struct cku_queue *queue )
{
	if( queue->v )
		free( queue->v );
	memset( queue, 0, sizeof(*queue) );
}

struct cku_queue *cku_queue_alloc( size_t n_alloc, size_t elem_len )
{
	struct cku_queue *queue = xalloc( sizeof(*queue) );
	cku_queue_ctor( queue, n_alloc, elem_len );
	return queue;
}

void cku_queue_free( struct cku_queue **queue )
{
	if( *queue == NULL ) return;

	cku_queue_dtor( *queue );
	free( *queue );
	*queue = NULL;
}


bool cku_queue_isempty( const struct cku_queue *queue )
{
	return ( queue->n_elem == 0 );
}

__inline__
static bool cku_queue_isfull( const struct cku_queue *queue )
{
	return ( queue->n_elem == queue->n_alloc );
}

void cku_queue_push( struct cku_queue *queue, const void *v )
{	
	if( cku_queue_isfull( queue ) ) {
		assert( cku_queue_end( queue ) == queue->front );
		resize_queue( queue );
	}
	
	// Copy to the vacant location
	memcpy( (void *)cku_queue_endptr( queue ), v, queue->elem_len );
	queue->n_elem++;
	
}

const void *cku_queue_pop( struct cku_queue *queue, void *v )
{
	if( cku_queue_isempty( queue ) ) 
		return NULL;

	const void *v_front = cku_queue_frontptr( queue );
	if( v != NULL ) memcpy( v, v_front, queue->elem_len );

	__queue_incr_front( queue );
	queue->n_elem--;

	return v_front;
}


// [0x] [1x] [2x] [3x] [4x] [o] [] [] [] [] [5x] 
//                      b    e               f
// [0x] [1x] [2x] [3x] [4x] [5x] [] [] [] [] [o]
// [5x] [1x] [2x] [3x] [4x] [0x] [] [] [] [] [o]
// [5x] [0x] [2x] [3x] [4x] [1x] [] [] [] [] [o]
// [5x] [0x] [1x] [3x] [4x] [2x] [] [] [] [] [o]
// [5x] [0x] [1x] [2x] [4x] [3x] [] [] [] [] [o]
// [5x] [0x] [1x] [2x] [3x] [4x] [] [] [] [] [o] 


// [0x] [1x] [2x] [3x] [4x] [o] [] [] [5x] [6x] [7x] 
//                      b    e            f


//  0    1    2    3    4    5    6    7
//                           0    1    2    
// [0x] [1x] [2x] [3x] [4x] [5x] [6x] [7x] [o] [] [] 
// [5x] [1x] [2x] [3x] [4x] [0x] [6x] [7x] [o] [] [] : i=0, j=0
// [5x] [6x] [2x] [3x] [4x] [0x] [1x] [7x] [o] [] [] : i=1, j=1
// [5x] [6x] [7x] [3x] [4x] [0x] [1x] [2x] [o] [] [] : i=2, j=2
// [5x] [6x] [7x] [0x] [4x] [3x] [1x] [2x] [o] [] [] : i=3, j=0
// [5x] [6x] [7x] [0x] [1x] [3x] [4x] [2x] [o] [] [] : i=4, j=1
// [5x] [6x] [7x] [0x] [1x] [2x] [4x] [3x] [o] [] [] : i=5, j=2

// [5x] [6x] [7x] [0x] [1x] [4x] [2x] [3x] [o] [] [] : i=6, j=0
// [5x] [6x] [7x] [0x] [1x] [4x] [3x] [2x] [o] [] [] : i=7, j=1

// [5x] [6x] [7x] [0x] [1x] [2x] [4x] [3x] [o] [] []



// [0x] [1x] [2x] [3x] [4x] [o] [] [] [] [] [5x] 
//                      b    e               f

// Shift front block to end index
// [0x] [1x] [2x] [3x] [4x] [5x] [] [] [] [] [o]

// Swap all front block with front of vector
// [5x] [1x] [2x] [3x] [4x] [0x] [] [] [] [] [o]

// Perform memcpy for all remaining (non-swapped) back block elements
// [5x] [1x] [2x] [3x] [4x] [0x] [1x] [2x] [3x] [4x] [o]

// Perform memmove to shift block to the left
// [5x] [0x] [1x] [2x] [3x] [4x] [] [] [] [] [o]

/* static void lshift_blk( void *v, void *blk_a, void *blk_b, size_t elem_len ) */
/* { */
	
/* } */

#define min( a, b ) ( a < b ? a : b )
#define max( a, b ) ( a > b ? a : b )


void cku_queue_linearize( struct cku_queue *queue )
{

	const unsigned int front = queue->front;
	const unsigned int back  = cku_queue_back( queue );

	if( front <= back ) return;

	const size_t blk_len = (queue->n_alloc - front) * queue->elem_len;
	void *buffer = xalloc( blk_len );

	// Store the trailing segment of the data vector into a buffer
	memcpy( buffer, cku_queue_frontptr(queue), blk_len );
	// Shift the remaining elements to make room for the front of the data vector
	memmove( queue->v + blk_len, queue->v, front * queue->elem_len );
	// Copy the front data vector block to the front
	memcpy( queue->v, buffer, blk_len );
	free(buffer);

	queue->front = 0;
}
