/*
 * Stack data structure module
 *
 * Copyright (C) 2017-2020 Jason Graham <jgraham@compukix.net>
 *
 * This file is part of libbds.
 *
 * libbds is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * libbds is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libbds. If not, see
 * <https://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memutil.h"
#include <libbds/bds_stack.h>

void bds_stack_construct(bds_stack_t *stack, size_t elem_len, void (*elem_dtor)(void *))
{
        bds_vector_construct((bds_vector_t *)stack, 0, elem_len, elem_dtor);
}

void bds_stack_destruct(bds_stack_t *stack) { bds_vector_destruct((bds_vector_t *)stack); }

bds_stack_t *bds_stack_create(size_t elem_len, void (*elem_dtor)(void *))
{
        return (bds_stack_t *)bds_vector_create(0, elem_len, elem_dtor);
}

void bds_stack_destroy(bds_stack_t **stack) { bds_vector_destroy((bds_vector_t **)stack); }

void bds_stack_push(bds_stack_t *stack, const void *v) { bds_vector_append((bds_vector_t *)stack, v); }

void *bds_stack_pop(bds_stack_t *stack, void *v)
{
        void *v_top = (void *)bds_stack_topptr(stack);
        if (v_top == NULL)
                return NULL;

        // Only perform the copy if a buffer is provided; otherwise we
        // are just poping and discarding
        if (v != NULL)
                memcpy(v, v_top, stack->vector__.elem_len);

        stack->vector__.n_elem--;

        return v_top;
}

void bds_stack_clear(bds_stack_t *stack) { bds_vector_clear((bds_vector_t *)stack); }

void *bds_stack_topptr(bds_stack_t *stack)
{
        return (void *)bds_stack_topptr_const((const bds_stack_t *)stack);
}

const void *bds_stack_topptr_const(const bds_stack_t *stack)
{
        if (bds_stack_isempty(stack))
                return NULL;

        return (const char *)stack->vector__.v + bds_stack_top(stack) * stack->vector__.elem_len;
}

void *bds_stack_lsearch(bds_stack_t *stack, const void *key, int (*compar)(const void *a, const void *b))
{
        return bds_vector_lsearch((bds_vector_t *)stack, key, compar);
}

const void *bds_stack_lsearch_const(const bds_stack_t *stack, const void *key,
                                    int (*compar)(const void *a, const void *b))
{
        return bds_vector_lsearch_const((const bds_vector_t *)stack, key, compar);
}

void *bds_stack_bsearch(bds_stack_t *stack, const void *key, int (*compar)(const void *a, const void *b))
{
        return bds_vector_bsearch((bds_vector_t *)stack, key, compar);
}

const void *bds_stack_bsearch_const(const bds_stack_t *stack, const void *key,
                                    int (*compar)(const void *a, const void *b))
{
        return bds_vector_bsearch_const((const bds_vector_t *)stack, key, compar);
}

void *bds_stack_modify(bds_stack_t *stack, const void *key, const void *v,
                       int (*compar)(const void *a, const void *b))
{
        void *v_key = (void *)bds_stack_lsearch(stack, key, compar);

        if (v_key == NULL)
                return NULL;
        memcpy(v_key, v, sizeof(stack->vector__.elem_len));

        return v_key;
}

void bds_stack_qsort(bds_stack_t *stack, int (*compar)(const void *, const void *))
{
        bds_vector_qsort((bds_vector_t *)stack, compar);
}
