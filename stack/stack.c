/** @file
 *  @brief Stack data structure module
 *
 *  @author Jason Graham <jgraha8@gmail.com>
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "memutil.h"
#include <ckunkwurx/cku_stack.h>


static void resize_stack( struct cku_stack *stack )
{
	const size_t len = ( stack->n_alloc * stack->elem_len );
	stack->v = xrealloc( stack->v, len, len << 1 );
	stack->n_elem <<= 1;
}

void cku_stack_ctor( struct cku_stack *stack, size_t n_alloc, size_t elem_len )
{
	memset( stack, 0, sizeof(*stack) );

	stack->n_alloc = n_alloc;
	stack->elem_len = elem_len;
	stack->v = xalloc( n_alloc * elem_len );
}

void cku_stack_dtor( struct cku_stack *stack )
{
	if( stack->v ) free( stack->v );
	memset(stack, 0, sizeof(*stack) );
}

void cku_stack_push( struct cku_stack *stack, const void *v )
{
	if( stack->n_elem == stack->n_alloc )
		resize_stack( stack );
	
	stack->n_elem++;
	memcpy( (void *)cku_stack_topptr(stack), v, stack->elem_len );
}

const void *cku_stack_pop( struct cku_stack *stack, void *v )
{
	int rc = 0;

	void *v_top = (void *)cku_stack_topptr( stack );
	if( v_top == NULL ) return NULL;

	// Only perform the copy if a buffer is provided; otherwise we
	// are just poping and discarding
	if( v != NULL ) 
		memcpy( v, v_top, stack->elem_len );

	stack->n_elem--;

	return v_top;
}

const void *cku_stack_topptr( const struct cku_stack *stack )
{
	if( cku_stack_isempty( stack ) )
		return NULL;
	
	return stack->v + cku_stack_top( stack ) * stack->elem_len;
}

const void *cku_stack_lsearch( const struct cku_stack *stack, const void *key,
			       int (*compar)( const void *a, const void *b ) )
{
	size_t i;
	const void *v;

	for( i=0; i<stack->n_elem; ++i ) {
		v = stack->v + i*stack->elem_len;
		if( compar( key, v ) == 0 ) return v;
	}
	return NULL;
}

int cku_stack_modify( struct cku_stack *stack, const void *key, const void *v,
		      int (*compar)( const void *a, const void *b ) )
{
	void *v_key = (void *)cku_stack_lsearch( stack, key, compar );

	if( v_key == NULL ) return -1;
	memcpy( v_key, v, sizeof(stack->elem_len) );

	return 0;
}
