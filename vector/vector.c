/** @file
 *  @brief Vector data structure module
 *
 *  @author Jason Graham <jgraha8@gmail.com>
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memutil.h"
#include <libbds/bds_vector.h>

void bds_vector_ctor(struct bds_vector *vector, size_t n_alloc, size_t elem_len, void (*elem_dtor)(void *))
{
        bds_stack_ctor((struct bds_stack *)vector, n_alloc, elem_len, elem_dtor);
}

void bds_vector_dtor(struct bds_vector *vector) { bds_stack_dtor((struct bds_stack *)vector); }

struct bds_vector *bds_vector_alloc(size_t n_alloc, size_t elem_len, void (*elem_dtor)(void *))
{
        return (struct bds_vector *)bds_stack_alloc(n_alloc, elem_len, elem_dtor);
}

void bds_vector_free(struct bds_vector **vector) { bds_stack_free((struct bds_stack **)vector); }

void bds_vector_append(struct bds_vector *vector, const void *v) { bds_stack_push((struct bds_stack *)vector, v); }

void bds_vector_clear(struct bds_vector *vector) { bds_stack_clear((struct bds_stack *)vector); }

const void *bds_vector_lsearch(const struct bds_vector *vector, const void *key,
                               int (*compar)(const void *a, const void *b))
{
        return bds_stack_lsearch((const struct bds_stack *)vector, key, compar);
}

const void *bds_vector_bsearch(const struct bds_vector *vector, const void *key,
                               int (*compar)(const void *a, const void *b))
{
        return bds_stack_bsearch((struct bds_stack *)vector, key, compar);
}

void bds_vector_qsort(struct bds_vector *vector, int (*compar)(const void *, const void *))
{
        bds_stack_qsort((struct bds_stack *)vector, compar);
}
