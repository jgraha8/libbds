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

inline static void __queue_incr_front( struct cku_queue *queue )
{
	queue->front = ( queue->front + 1 ) % queue->n_alloc;
}

inline static size_t __queue_end( const struct cku_queue *queue )
{
	assert( queue->n_alloc > 0 );
	return ( ( cku_queue_back( queue ) + 1 ) % queue->n_alloc );
}

inline static const void *__queue_endptr( const struct cku_queue *queue )
{
	return (const void *)( queue->v + __queue_end( queue ) * queue->elem_len );
}


void cku_queue_ctor( struct cku_queue *queue, size_t n_alloc, size_t elem_len )
{
	memset(queue, 0, sizeof(*queue));

	queue->n_alloc   = n_alloc;
	queue->elem_len = elem_len;

	queue->v = xalloc( n_alloc * elem_len );
}

void cku_queue_dtor( struct cku_queue *queue, void (*elem_dtor)(void *) )
{
	if( queue->v != NULL ) {
		cku_queue_clear( queue, elem_dtor );
		free( queue->v );
	}
	memset( queue, 0, sizeof(*queue) );
}

struct cku_queue *cku_queue_alloc( size_t n_alloc, size_t elem_len )
{
	struct cku_queue *queue = xalloc( sizeof(*queue) );
	cku_queue_ctor( queue, n_alloc, elem_len );
	return queue;
}

void cku_queue_free( struct cku_queue **queue, void (*elem_dtor)(void *) )
{
	if( *queue == NULL ) return;

	cku_queue_dtor( *queue, elem_dtor );
	free( *queue );
	*queue = NULL;
}

void cku_queue_resize( struct cku_queue *queue )
{
	const size_t n_alloc   = queue->n_alloc << 1; // Double the size (need overflow check here)
	const size_t elem_len = queue->elem_len;
	
	queue->v = xrealloc( queue->v, queue->n_alloc * elem_len, n_alloc * elem_len );

	cku_queue_linearize( queue );
	queue->n_alloc  = n_alloc;
}

void cku_queue_push( struct cku_queue *queue, const void *v )
{	
	if( cku_queue_isfull( queue ) ) {
		assert( __queue_end( queue ) == queue->front );
		if( queue->auto_resize ) {
			cku_queue_resize( queue );
		} else {
			fprintf(stderr,"error: cku::cku_queue_push queue is full and auto_resize = false: cannot perform push\n");
			exit(EXIT_FAILURE);
		}
	}
	
	// Copy to the vacant location
	memcpy( (void *)__queue_endptr( queue ), v, queue->elem_len );
	queue->n_elem++;
	
}

void *cku_queue_pop( struct cku_queue *queue, void *v )
{
	void *v_front = (void *)cku_queue_frontptr( queue );
	if( v_front == NULL ) return NULL;
	
	if( v != NULL ) memcpy( v, v_front, queue->elem_len );

	__queue_incr_front( queue );
	queue->n_elem--;

	return v_front;
}

void cku_queue_clear( struct cku_queue *queue, void (*elem_dtor)(void *) )
{
	if( elem_dtor != NULL ) {
		while( !cku_queue_isempty( queue ) ) {
			elem_dtor( cku_queue_pop( queue, NULL ) );
		}
	}
	queue->front=0;
	queue->n_elem=0;
}

void cku_queue_clear_nfront( struct cku_queue *queue, size_t n_clear, void (*elem_dtor)(void *) )
{
	if( cku_queue_size(queue) < n_clear ) {
		printf("warning: cku::cku_queue_clear_nfront: n_clear larger than size of queue\n");
		cku_queue_clear( queue, elem_dtor );
		return;
	}

	if( elem_dtor != NULL ) {
		size_t n;
		for( n = 0; n<n_clear; ++n ) {
			elem_dtor( cku_queue_pop( queue, NULL ) );
		}
	} else {
		queue->front = ( queue->front + n_clear ) % queue->n_alloc;
		queue->n_elem -= n_clear;
	}
}

void cku_queue_linearize( struct cku_queue *queue )
{
	
	const size_t front = queue->front;
	const size_t back  = cku_queue_back( queue );

	if( CKU_QUEUE_ISLINEAR( front, back ) ) return;

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

const void *cku_queue_lsearch( const struct cku_queue *queue, const void *key,
			       int (*compar)( const void *, const void *) )
{
	size_t i,j;
	const void *v;

	j=queue->front;
	for( i=0; i<queue->n_elem; ++i ) {
		v = queue->v + j*queue->elem_len;
		if( compar( key, v ) == 0 ) return v;
		j = ( j + 1 ) % queue->n_alloc;
	}
	return NULL;
}
