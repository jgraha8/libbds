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

int cku_stack_gettop( struct cku_stack *stack, void *v );

bool cku_stack_isempty( struct cku_stack *stack );

#endif // __CKU_STACK_H__
