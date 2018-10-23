/** @file
 *  @brief Stack data structure module definitions
 *
 *  @author Jason Graham <jgraha8@gmail.com>
 */

#ifndef __BDS_STACK_H__
#define __BDS_STACK_H__

#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Stack data structure
 */
struct bds_stack {
        size_t n_alloc;            ///< Number of elements allocated in the stack data vector
        size_t elem_len;           ///< Length in bytes of each data element
        size_t n_elem;             ///< Number of elements in the stack
        void *v;                   ///< Address of stack data vector
        void (*elem_dtor)(void *); ///< Destructor for each data element in data vector
};

/**
 * @brief Constructor for the stack data structure
 *
 * @param stack Address of the stack object
 * @param n_alloc Number of data elements to allocate
 * @param elem_len Length in bytes of each data element
 * @param elem_dtor Destructor for each data element (disabled if NULL)
 */
void bds_stack_ctor(struct bds_stack *stack, size_t n_alloc, size_t elem_len, void (*elem_dtor)(void *));

/**
 * @brief Destructor for the stack data structure
 * @param stack Address of the stack object
 * @param elem_dtor Destructor for each data element (disabled if NULL)
 */
void bds_stack_dtor(struct bds_stack *stack);

/**
 * @brief Allocator for the stack data structure
 *
 * @param n_alloc Number of data elements to allocate
 * @param elem_len Length in bytes of each data element
 * @param elem_dtor Destructor for each data element (disabled if NULL)
 * @retval Address of stack object
 */
struct bds_stack *bds_stack_alloc(size_t n_alloc, size_t elem_len, void (*elem_dtor)(void *));

/**
 * @brief Frees the stack data structrue
 *
 * @param stack Address of stack object address (sets the stack object address to @c NULL)
 */
void bds_stack_free(struct bds_stack **stack);

/**
 * @brief Pushes a data element onto the stack
 *
 * @param stack Address of the stack object
 * @param v Address of the data element to be pushed onto the stack
 */
void bds_stack_push(struct bds_stack *stack, const void *v);

/**
 * @brief Pops a data element off of the stack
 *
 * @param stack Address of the stack object
 * @param v Address of a buffer to copy the popped data element (if @c NULL then the element is not copied)
 * @retval Address of the internal stack vector of popped data element (returns @c NULL if the stack is empty)
 */
void *bds_stack_pop(struct bds_stack *stack, void *v);

/**
 * @brief Provides the number of elements in the stack
 *
 * @param stack Address of stack object
 */
static inline size_t bds_stack_size(const struct bds_stack *stack) { return stack->n_elem; }

/**
 * @brief Gets the index of the top of the stack (index of last pushed element)
 *
 * @param stack Address of the stack object
 * @retval Index of top element in the stack (returns -1 if the stack is empty)
 */
static inline ssize_t bds_stack_top(const struct bds_stack *stack) { return ((ssize_t)stack->n_elem - 1); }

/**
 * @brief Gets the address of top data element
 *
 * @param stack Address of the stack object
 * @retval Address of the internal stack vector of the top data element (returns @c NULL if the stack is empty)
 */
const void *bds_stack_topptr(const struct bds_stack *stack);

/**
 * @brief Gets the address of stack data vector
 *
 * @param stack Address of the stack object
 * @retval Address of the internal stack vector
 */
static inline const void *bds_stack_ptr(const struct bds_stack *stack) { return (const void *)stack->v; }

/**
 * @brief Tests if the stack is empty
 *
 * @param stack Address of the stack object
 * @retval Returns true if the stack is empty; false otherwise
 */
static inline bool bds_stack_isempty(const struct bds_stack *stack) { return (stack->n_elem == 0); }

/**
 * @brief Tests if the stack is full
 *
 * @param stack Address of the stack object
 * @retval Returns true if the stack is full; false otherwise
 */
static inline bool bds_stack_isfull(const struct bds_stack *stack) { return (stack->n_elem == stack->n_alloc); }

/**
 * @brief Clears the stack (does not deallocate memory)
 *
 * @param stack Address of the stack object
 */
void bds_stack_clear(struct bds_stack *stack);

/**
 * @brief Peforms a linear search for a data element
 *
 * @param stack Address of the stack object
 * @param key Address of the key object
 * @param compar Address of the comparator function
 */
const void *bds_stack_lsearch(const struct bds_stack *stack, const void *key,
                              int (*compar)(const void *, const void *));

/**
 * @brief Peforms a binary search for a data element using bsearch
 *
 * @param stack Address of the stack object
 * @param key Address of the key object
 * @param compar Address of the comparator function
 */
const void *bds_stack_bsearch(const struct bds_stack *stack, const void *key,
                              int (*compar)(const void *a, const void *b));

/**
 * @brief Modifies a stack data element
 *
 * @param stack Address of the stack object
 * @param key Address of the key object
 * @param v Address of new object (copied to element matching @c key)
 * @param compar Address of the comparator function
 */
const void *bds_stack_modify(struct bds_stack *stack, const void *key, const void *v,
                             int (*compar)(const void *, const void *));

/**
 * @brief Sorts all elements in the stack using qsort
 *
 * @param stack Address of the stack object
 * @param compar Address of the comparator function
 */
void bds_stack_qsort(struct bds_stack *stack, int (*compar)(const void *, const void *));

#ifdef BDS_NAMESPACE
typedef struct bds_stack stack_t;
#define stack_ctor bds_stack_ctor
#define stack_dtor bds_stack_dtor
#define stack_push bds_stack_push
#define stack_pop bds_stack_pop
#define stack_size bds_stack_size
#define stack_top bds_stack_top
#define stack_topptr bds_stack_topptr
#define stack_ptr bds_stack_ptr
#define stack_isempty bds_stack_isempty
#define stack_isfull bds_stack_isfull
#define stack_clear bds_stack_clear
#define stack_lsearch bds_stack_lsearch
#define stack_bsearch bds_stack_bsearch
#define stack_modify bds_stack_modify
#define stack_qsort bds_stack_qsort
#endif

#ifdef __cplusplus
}
#endif

#endif // __BDS_STACK_H__
