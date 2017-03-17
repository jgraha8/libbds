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

/**
 * @brief Stack data structure
 */
struct cku_stack {
	size_t n_alloc;  ///< Number of elements allocated in the stack data vector
	size_t elem_len; ///< Length in bytes of each data element
	size_t n_elem;   ///< Number of elements in the stack
	void *v;         ///< Address of stack data vector
};

/**
 * @brief Constructor for the stack data structure
 *
 * @param stack Address of the stack object
 * @param n_alloc Number of data elements to allocate
 * @param elem_len Length in bytes of each data element
 */
void cku_stack_ctor( struct cku_stack *stack, size_t n_alloc, size_t elem_len );

/**
 * @brief Destructor for the stack data structure
 */
void cku_stack_dtor( struct cku_stack *stack );

/**
 * @brief Pushes a data element onto the stack
 *
 * @param stack Address of the stack object
 * @param v Address of the data element to be pushed onto the stack
 */
void cku_stack_push( struct cku_stack *stack, const void *v );

/**
 * @brief Pops a data element off of the stack
 *
 * @param stack Address of the stack object
 * @param v Address of a buffer to copy the popped data element (if @c NULL then the element is not copied)
 * @retval Address of the internal stack vector of popped data element (returns @c NULL if the stack is empty)
 */
const void *cku_stack_pop( struct cku_stack *stack, void *v );

/**
 * @brief Gets the index of the top of the stack (index of last pushed element)
 *
 * @param stack Address of the stack object
 * @retval Index of top element in the stack (returns -1 if the stack is empty)
 */
static inline ssize_t cku_stack_top( const struct cku_stack *stack )
{
	return ((ssize_t)stack->n_elem - 1);
}

/**
 * @brief Gets the address of top data element
 *
 * @param stack Address of the stack object
 * @retval Address of the internal stack vector of the top data element (returns @c NULL if the stack is empty)
 */
const void *cku_stack_topptr( const struct cku_stack *stack );

/**
 * @brief Gets the address of stack data vector
 *
 * @param stack Address of the stack object
 * @retval Address of the internal stack vector
 */
static inline const void *cku_stack_ptr( const struct cku_stack *stack )
{
	return (const void *)stack->v;
}

/**
 * @brief Tests if the stack is empty
 *
 * @param stack Address of the stack object
 * @retval Returns true if the stack is empty; false otherwise
 */
static inline bool cku_stack_isempty( const struct cku_stack *stack )
{
	return ( stack->n_elem == 0 );
}

/**
 * @brief Tests if the stack is full
 *
 * @param stack Address of the stack object
 * @retval Returns true if the stack is full; false otherwise
 */
static inline bool cku_stack_isfull( const struct cku_stack *stack )
{
	return ( stack->n_elem == stack->n_alloc );
}

/**
 * @brief Clears the stack (does not deallocate memory)
 *
 * @param stack Address of the stack object
 */
static inline void cku_stack_clear( struct cku_stack *stack )
{
	stack->n_elem = 0;
}

/**
 * @brief Peforms a linear search for a data element
 *
 * @param stack Address of the stack object
 * @param key Address of the key object
 * @param compar Address of the comparator function
 */
const void *cku_stack_lsearch( const struct cku_stack *stack, const void *key,
			       int (*compar)( const void *, const void *) );

/**
 * @brief Peforms a binary search for a data element using bsearch
 *
 * @param stack Address of the stack object
 * @param key Address of the key object
 * @param compar Address of the comparator function
 */
const void *cku_stack_bsearch( const struct cku_stack *stack, const void *key,
			       int (*compar)( const void *a, const void *b ) );

/**
 * @brief Modifies a stack data element
 *
 * @param stack Address of the stack object
 * @param key Address of the key object
 * @param v Address of new object (copied to element matching @c key)
 * @param compar Address of the comparator function
 */
const void *cku_stack_modify( struct cku_stack *stack, const void *key, const void *v,
			      int (*compar)( const void *, const void *) );

/**
 * @brief Sorts all elements in the stack using qsort
 *
 * @param stack Address of the stack object
 * @param compar Address of the comparator function
 */
void cku_stack_qsort( struct cku_stack *stack,
		      int (*compar)( const void *, const void *) );

#endif // __CKU_STACK_H__
