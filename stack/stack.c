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
#include <libbds/bds_stack.h>


static void resize_stack( struct bds_stack *stack )
{
	const size_t len = ( stack->n_alloc * stack->elem_len );
	stack->v = xrealloc( stack->v, len, len << 1 );
	stack->n_alloc <<= 1;
}

void bds_stack_ctor( struct bds_stack *stack, size_t n_alloc, size_t elem_len )
{
	memset( stack, 0, sizeof(*stack) );
	assert( n_alloc > 0 );

	stack->n_alloc = n_alloc;
	stack->elem_len = elem_len;
	stack->v = xalloc( n_alloc * elem_len );
}

void bds_stack_dtor( struct bds_stack *stack, void (*elem_dtor)(void *) )
{
	if( stack->v != NULL ) {
		bds_stack_clear( stack, elem_dtor );
		free( stack->v );
	}
	memset(stack, 0, sizeof(*stack) );
}

void bds_stack_push( struct bds_stack *stack, const void *v )
{
	if( stack->n_elem == stack->n_alloc )
		resize_stack( stack );
	
	stack->n_elem++;
	memcpy( (void *)bds_stack_topptr(stack), v, stack->elem_len );
}

void *bds_stack_pop( struct bds_stack *stack, void *v )
{
	void *v_top = (void *)bds_stack_topptr( stack );
	if( v_top == NULL ) return NULL;

	// Only perform the copy if a buffer is provided; otherwise we
	// are just poping and discarding
	if( v != NULL ) 
		memcpy( v, v_top, stack->elem_len );

	stack->n_elem--;

	return v_top;
}

void bds_stack_clear( struct bds_stack *stack, void (*elem_dtor)(void *) )
{
	if ( elem_dtor != NULL ) {
		while( !bds_stack_isempty( stack ) ) {
			elem_dtor( bds_stack_pop( stack, NULL ) );
		}		
	}
	stack->n_elem = 0;
}

const void *bds_stack_topptr( const struct bds_stack *stack )
{
	if( bds_stack_isempty( stack ) )
		return NULL;
	
	return stack->v + bds_stack_top( stack ) * stack->elem_len;
}

const void *bds_stack_lsearch( const struct bds_stack *stack, const void *key,
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

const void *bds_stack_bsearch( const struct bds_stack *stack, const void *key,
			       int (*compar)( const void *a, const void *b ) )
{
	return bsearch( key, stack->v, stack->n_elem, stack->elem_len, compar );
}

const void *bds_stack_modify( struct bds_stack *stack, const void *key, const void *v,
			      int (*compar)( const void *a, const void *b ) )
{
	void *v_key = (void *)bds_stack_lsearch( stack, key, compar );

	if( v_key == NULL ) return NULL;
	memcpy( v_key, v, sizeof(stack->elem_len) );

	return v_key;
}

void bds_stack_qsort( struct bds_stack *stack,
		      int (*compar)( const void *, const void *) )
{
	qsort(stack->v, stack->n_elem, stack->elem_len, compar );	
}
