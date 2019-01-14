/*
 * Vector data structure module
 *
 * Copyright (C) 2018-2019 Jason Graham <jgraham@compukix.net>
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
#include <libbds/bds_vector.h>
#include <libbds/bds_stack.h>

static void resize_vector(struct bds_vector *vector)
{
        const size_t len = (vector->n_alloc * vector->elem_len);
        vector->v        = xrealloc(vector->v, len, len << 1);
        vector->n_alloc <<= 1;
}

void bds_vector_ctor(struct bds_vector *vector, size_t n_alloc, size_t elem_len, void (*elem_dtor)(void *))
{
        memset(vector, 0, sizeof(*vector));
        assert(n_alloc > 0);

        vector->n_alloc   = n_alloc;
        vector->elem_len  = elem_len;
        vector->v         = xalloc(n_alloc * elem_len);
        vector->elem_dtor = elem_dtor;
}

void bds_vector_dtor(struct bds_vector *vector)
{
        if (vector->v != NULL) {
                bds_vector_clear(vector);
                free(vector->v);
        }
        memset(vector, 0, sizeof(*vector));
}

struct bds_vector *bds_vector_alloc(size_t n_alloc, size_t elem_len, void (*elem_dtor)(void *))
{
        struct bds_vector *vector = malloc(sizeof(*vector));
        assert(vector);

        bds_vector_ctor(vector, n_alloc, elem_len, elem_dtor);
        return vector;
}

void bds_vector_free(struct bds_vector **vector)
{
        if (*vector == NULL)
                return;

        bds_vector_dtor(*vector);
        free(*vector);
        *vector = NULL;
}

void bds_vector_append(struct bds_vector *vector, const void *v)
{
        if (vector->n_elem == vector->n_alloc)
                resize_vector(vector);

        memcpy((char *)vector->v + vector->n_elem * vector->elem_len, v, vector->elem_len);
        vector->n_elem++;
}

void bds_vector_clear(struct bds_vector *vector)
{
        if (vector->elem_dtor != NULL) {
                while (!bds_vector_isempty(vector)) {
                        vector->elem_dtor(bds_stack_pop((struct bds_stack *)vector, NULL));
                }
        }
        vector->n_elem = 0;
}

const void *bds_vector_lsearch(const struct bds_vector *vector, const void *key,
                               int (*compar)(const void *a, const void *b))
{
        size_t i;
        const void *v;

        for (i = 0; i < vector->n_elem; ++i) {
                v = (char *)vector->v + i * vector->elem_len;
                if (compar(key, v) == 0)
                        return v;
        }
        return NULL;
}

const void *bds_vector_bsearch(const struct bds_vector *vector, const void *key,
                               int (*compar)(const void *a, const void *b))
{
        return bsearch(key, vector->v, vector->n_elem, vector->elem_len, compar);
}

void bds_vector_qsort(struct bds_vector *vector, int (*compar)(const void *, const void *))
{
        qsort(vector->v, vector->n_elem, vector->elem_len, compar);
}
