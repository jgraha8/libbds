/** @file
 *  @brief Stack data structure module definitions
 *
 *  @author Jason Graham <jgraha8@gmail.com>
 */

#ifndef __CKU_STACK_H__
#define __CKU_STACK_H__

#include <stdlib.h>
#include <stdbool.h>

struct cku_stack {
	unsigned long n_elem;
	size_t elem_len;
	void *v;
	unsigned long i_top;	
};

void cku_stack_ctor( struct cku_stack *stack, unsigned long n_elem, size_t elem_len );

void cku_stack_dtor( struct cku_stack *stack );

void cku_stack_push( struct cku_stack *stack, const void *v );

int cku_stack_pop( struct cku_stack *stack, void *v );

const void *cku_stack_gettop( const struct cku_stack *stack );

__inline__
static const void *cku_stack_get( const struct cku_stack *stack )
{
	return (const void *)stack->v;
}

__inline__
static bool cku_stack_isempty( const struct cku_stack *stack )
{
	return ( stack->i_top == -1 );
}

__inline__
static void cku_stack_clear( struct cku_stack *stack )
{
	stack->i_top = -1;
}

const void *cku_stack_lsearch( const struct cku_stack *stack, const void *key, int (*compar)( const void *, const void *) );

int cku_stack_modify( struct cku_stack *stack, const void *key, const void *v, int (*compar)( const void *, const void *) );



#endif // __CKU_STACK_H__
