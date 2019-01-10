/** @file
 *  @brief Stack data structure module
 *
 *  @author Jason Graham <jgraha8@gmail.com>
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memutil.h"
#include <libbds/bds_stack.h>

void bds_stack_ctor(struct bds_stack *stack, size_t n_alloc, size_t elem_len, void (*elem_dtor)(void *))
{
        bds_vector_ctor((struct bds_vector *)stack, n_alloc, elem_len, elem_dtor);
}

void bds_stack_dtor(struct bds_stack *stack) { bds_vector_dtor((struct bds_vector *)stack); }

struct bds_stack *bds_stack_alloc(size_t n_alloc, size_t elem_len, void (*elem_dtor)(void *))
{
        return (struct bds_stack *)bds_vector_alloc(n_alloc, elem_len, elem_dtor);
}

void bds_stack_free(struct bds_stack **stack) { bds_vector_free((struct bds_vector **)stack); }

void bds_stack_push(struct bds_stack *stack, const void *v) { bds_vector_append((struct bds_vector *)stack, v); }

void *bds_stack_pop(struct bds_stack *stack, void *v)
{
        void *v_top = (void *)bds_stack_topptr(stack);
        if (v_top == NULL)
                return NULL;

        // Only perform the copy if a buffer is provided; otherwise we
        // are just poping and discarding
        if (v != NULL)
                memcpy(v, v_top, stack->__vector.elem_len);

        stack->__vector.n_elem--;

        return v_top;
}

void bds_stack_clear(struct bds_stack *stack) { bds_vector_clear((struct bds_vector *)stack); }

const void *bds_stack_topptr(const struct bds_stack *stack)
{
        if (bds_stack_isempty(stack))
                return NULL;

        return (char *)stack->__vector.v + bds_stack_top(stack) * stack->__vector.elem_len;
}

const void *bds_stack_lsearch(const struct bds_stack *stack, const void *key,
                              int (*compar)(const void *a, const void *b))
{
        return bds_vector_lsearch((const struct bds_vector *)stack, key, compar);
}

const void *bds_stack_bsearch(const struct bds_stack *stack, const void *key,
                              int (*compar)(const void *a, const void *b))
{
        return bds_vector_bsearch((struct bds_vector *)stack, key, compar);
}

const void *bds_stack_modify(struct bds_stack *stack, const void *key, const void *v,
                             int (*compar)(const void *a, const void *b))
{
        void *v_key = (void *)bds_stack_lsearch(stack, key, compar);

        if (v_key == NULL)
                return NULL;
        memcpy(v_key, v, sizeof(stack->__vector.elem_len));

        return v_key;
}

void bds_stack_qsort(struct bds_stack *stack, int (*compar)(const void *, const void *))
{
        bds_vector_qsort((struct bds_vector *)stack, compar);
}
