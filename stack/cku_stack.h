/** @file
 *  @brief Stack data structure module definitions
 *
 *  @author Jason Graham <jgraha8@gmail.com>
 */

#ifndef __CKU_STACK_H__
#define __CKU_STACK_H__

#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
struct cku_stack {
	size_t n_alloc;
	size_t elem_len;
	size_t n_elem;
	void *v;

};

void cku_stack_ctor( struct cku_stack *stack, size_t n_alloc, size_t elem_len );

void cku_stack_dtor( struct cku_stack *stack );

void cku_stack_push( struct cku_stack *stack, const void *v );

const void *cku_stack_pop( struct cku_stack *stack, void *v );

__inline__
static ssize_t cku_stack_top( const struct cku_stack *stack )
{
	return ((ssize_t)stack->n_elem - 1);
}

const void *cku_stack_topptr( const struct cku_stack *stack );

__inline__
static const void *cku_stack_ptr( const struct cku_stack *stack )
{
	return (const void *)stack->v;
}

__inline__
static bool cku_stack_isempty( const struct cku_stack *stack )
{
	return ( stack->n_elem == 0 );
}

__inline__
static void cku_stack_clear( struct cku_stack *stack )
{
	stack->n_elem = 0;
}

const void *cku_stack_lsearch( const struct cku_stack *stack, const void *key,
			       int (*compar)( const void *, const void *) );

int cku_stack_modify( struct cku_stack *stack, const void *key, const void *v,
		      int (*compar)( const void *, const void *) );



#endif // __CKU_STACK_H__
