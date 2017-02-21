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

void cku_queue_push( struct cku_queue *queue, const void *v )
{
	if( queue->n_elem == queue->n_alloc ) {
		assert( (queue->i_back + 1) % queue->n_alloc == queue->i_front );
		resize_queue( queue );
	}

	// Get index of vacant
	__queue_incr_back( queue );
	
	// Copy to the vacant location
	memcpy( queue->v + queue->i_back * queue->elem_len,
		v,
		queue->elem_len );
	
	++queue->n_elem;
}

const void *cku_queue_pop( struct cku_queue *queue, void *v )
{
	if( cku_queue_isempty( queue ) ) 
		return NULL;

	const void *v_front = queue->v + queue->i_front * queue->elem_len;
	if( v != NULL ) memcpy( v, v_front, queue->elem_len );

	__queue_incr_front( queue );
	queue->n_elem--;

	return v_front;
}



// [0x] [1x] [2x] [3o] [4o] [5o] [6x] [7x] [8x]
//            b                    f     
// [6x] [1x] [2x] [3o] [4o] [5o] [0x] [7x] [8x]
// [6x] [7x] [2x] [3o] [4o] [5o] [0x] [1x] [8x]
// [6x] [7x] [8x] [3o] [4o] [5o] [0x] [1x] [2x]
// [6x] [7x] [8x] [0x] [4o] [5o] [3o] [1x] [2x]
// [6x] [7x] [8x] [0x] [1x] [5o] [3o] [4o] [2x]
// [6x] [7x] [8x] [0x] [1x] [2x] [3o] [4o] [5o]


// [0x] [1x] [2x] [3x] [4o] [5o] [6o] [7o]
//             f    b   e               
// [2x] [1x] [0x] [3x] [4o] [5o] [6o] [7o]
// [2x] [3x] [0x] [1x] [4o] [5o] [6o] [7o]

// [0x] [1x] [2x] [3o] [4o] [5o] [6x] [7x]
//             b    e             f     

// [6x] [1x] [2x] [3o] [4o] [5o] [0x] [7x]
// [6x] [7x] [2x] [3o] [4o] [5o] [0x] [1x]
// [6x] [7x] [0x] [3o] [4o] [5o] [2x] [1x]
// [6x] [7x] [0x] [1x] [4o] [5o] [2x] [3o]
// [6x] [7x] [0x] [1x] [2x] [5o] [4o] [3o]

// [0x] [1x] [2x] [3o] [4o] [5o] [6x] [7x]
//             b    e             f  
 /* static void lshift_blk( void *v, void *blk_a, void *blk_b, size_t elem_len ) */
/* { */
	
/* } */


void cku_queue_linearize( struct cku_queue *queue )
{

	const unsigned int i_end = cku_queue_iend( queue );

	
	if( queue->i_front < i_end ) return;
	
	const size_t elem_len     = queue->elem_len;
	const unsigned int n_swap = modulol( i_end - queue->i_front, queue->n_alloc );
	
	unsigned int j = queue->i_front;

	for( i=0; i<n_swap; ++i ) {
		void *vi = queue->v + i*elem_len;
		void *vj = queue->v + j*elem_len;
		xmemswap( vi, vj, elem_len );

		j = ( j + 1 ) % queue->n_alloc;
	}

	queue->i_front = 0;
	queue->i_back  = queue->n_elem - 1;
}
