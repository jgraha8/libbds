#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <ckunkwurx/cku_queue.h>
#include "memutil.h"

static void resize_queue( struct cku_queue *queue )
{
	const size_t n_elem   = queue->n_elem << 1; // Double the size
	const size_t elem_len = queue->elem_len;
	
	queue->v = xrealloc( queue->v, queue->n_elem * elem_len, n_elem * elem_len );

	// Swap elements until the we are at index 0, starting at index i_begin
	size_t i = 0;
	size_t j = queue->i_begin;

	while( j != 0 ) {
		void *vi = queue->v + i*elem_len;
		void *vj = queue->v + j*elem_len;
		xmemswap( vi, vj, elem_len );

		++i;
		j = ( j + 1 ) % queue->n_elem;
	}
	
	queue->i_begin = 0;
	queue->i_end   = queue->n_elem;
	queue->n_elem  = n_elem;
	
}

void cku_queue_ctor( struct cku_queue *queue, size_t n_elem, size_t elem_len )
{
	memset(queue, 0, sizeof(*queue));

	queue->n_elem   = n_elem;
	queue->elem_len = elem_len;

	queue->v = xalloc( n_elem * elem_len );

	queue->i_begin = 0;
	queue->i_end   = 0;
}

void cku_queue_dtor( struct cku_queue *queue )
{
	if( queue->v )
		free( queue->v );
	memset( queue, 0, sizeof(*queue) );
}

struct cku_queue *cku_queue_alloc( size_t n_elem, size_t elem_len )
{
	struct cku_queue *queue = xalloc( sizeof(*queue) );
	cku_queue_ctor( queue, n_elem, elem_len );
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
	return ( queue->i_begin == queue->i_end );
}

void cku_queue_push( struct cku_queue *queue, const void *v )
{
	// Copy to the vacant location
	memcpy( queue->v + queue->i_end * queue->elem_len,
		v,
		queue->elem_len );
	
	queue->i_end = (queue->i_end + 1) % queue->n_elem;

	if( queue->i_begin == queue->i_end )
		resize_queue( queue );
}

int cku_queue_pop( struct cku_queue *queue, void *v )
{
	if( cku_queue_isempty( queue ) ) 
		return 1;

	memcpy( v,
		queue->v + queue->i_begin * queue->elem_len,
		queue->elem_len );
	
	++queue->i_begin;

	return 0;
}
