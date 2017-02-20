/** @file
 *  @brief Stack data structure module
 *
 *  @author Jason Graham <jgraha8@gmail.com>
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <ckunkwurx/cku_stack.h>
#include "memutil.h"

static void resize_stack( struct cku_stack *stack )
{
	const size_t len = ( stack->n_elem * stack->elem_len );
	stack->v = xrealloc( stack->v, len, len << 1 );
	stack->n_elem <<= 1;
}

void cku_stack_ctor( struct cku_stack *stack, unsigned long n_elem, size_t elem_len )
{
	memset( stack, 0, sizeof(*stack) );

	stack->i_top = -1;
	stack->n_elem = n_elem;
	stack->elem_len = elem_len;
	stack->v = xalloc( n_elem * elem_len );
}

void cku_stack_dtor( struct cku_stack *stack )
{
	if( stack->v ) free( stack->v );
	memset(stack, 0, sizeof(*stack) );
}

void cku_stack_push( struct cku_stack *stack, const void *v )
{
	if( stack->i_top == stack->n_elem - 1 )
		resize_stack( stack );
	
	++stack->i_top;
	memcpy( stack->v + stack->i_top * stack->elem_len, v, stack->elem_len );
}

int cku_stack_pop( struct cku_stack *stack, void *v )
{
	int rc = 0;

	rc = cku_stack_gettop( stack, v );
	if( rc != 0 ) return rc;

	--stack->i_top;

	return 0;
}

bool cku_stack_isempty( struct cku_stack *stack )
{
	return ( stack->i_top == -1 );
}

int cku_stack_gettop( struct cku_stack *stack, void *v )
{
	if( cku_stack_isempty( stack ) )
		return 1;
	
	memcpy( v, stack->v + stack->i_top*stack->elem_len, stack->elem_len );

	return 0;
}
