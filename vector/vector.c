/*
 * Vector data structure module
 *
 * Copyright (C) 2018-2020 Jason Graham <jgraham@compukix.net>
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
#include <libbds/bds_vector.h>

/*
 * Defines a data array that can be indexed as __data[i] 
 */
#define DEFINE_DATA_ARRAY(__data, __vector) char (*__data)[(__vector)->elem_len] = (char (*)[(__vector)->elem_len])(__vector)->v

static void realloc_vector(struct bds_vector *vector, size_t num_alloc)
{
        const size_t prev_len = (vector->n_alloc * vector->elem_len);
	const size_t len = num_alloc * vector->elem_len;
        vector->v        = xrealloc(vector->v, prev_len, len);
        vector->n_alloc = num_alloc;
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

size_t bds_vector_resize(struct bds_vector *vector, size_t num_elem)
{
	if( num_elem <= vector->n_elem ) {
		/*
		  Shrinking size (or the same size)
		*/
		// Destroy old elements
		if( vector->elem_dtor ) {
			for( size_t i = num_elem; i<vector->n_elem; ++i ) {
				vector->elem_dtor(bds_vector_get(vector,i));
			}
		}
	} else {
		/*
		  Growing size
		*/
		size_t num_alloc = vector->n_alloc;
		while( num_elem > num_alloc ) {
			num_alloc <<= 1;
		}
		realloc_vector(vector, num_alloc);
	}

	vector->n_elem = num_elem;
	return vector->n_elem;
}


void bds_vector_append(struct bds_vector *vector, const void *v)
{
        if (vector->n_elem == vector->n_alloc)
                realloc_vector(vector, vector->n_alloc << 1);

        memcpy((char *)vector->v + vector->n_elem * vector->elem_len, v, vector->elem_len);
        vector->n_elem++;
}

void bds_vector_insert_sort(struct bds_vector *vector, const void *elem, int (*compar)(const void *, const void *))
{
	bds_vector_append(vector, elem);
	assert( vector->n_elem > 0 );
	
	if( 1 == vector->n_elem ) {
		return;
	}

	DEFINE_DATA_ARRAY(data, vector);
	
	size_t i = vector->n_elem - 1;
	char tmp[vector->elem_len];
	
	while( i > 0 && compar(data[i], data[i-1]) < 0) {
		memcpy(tmp, data[i-1], vector->elem_len);
		memcpy(data[i-1], data[i], vector->elem_len);
		memcpy(data[i], tmp, vector->elem_len);
		--i;
	}
}

void bds_vector_remove(struct bds_vector *vector, size_t i)
{
	if( vector->n_elem == 0 )
		return;
	
	size_t num_move = (vector->n_elem - 1) - i;

	char *dst = (char *)vector->v + i * vector->elem_len;
	char *src = dst + vector->elem_len;

	if( vector->elem_dtor ) {
		vector->elem_dtor(dst);
	}

	memmove(dst, src, num_move * vector->elem_len );
	--vector->n_elem;
}

void bds_vector_clear(struct bds_vector *vector)
{
        if (vector->elem_dtor != NULL) {
		for( size_t i=0; i<vector->n_elem; ++i ) {
			vector->elem_dtor(bds_vector_get(vector, i));
		}
        }
        vector->n_elem = 0;
}

void *bds_vector_lsearch(struct bds_vector *vector, const void *key, int (*compar)(const void *a, const void *b))
{
        return (void *)bds_vector_lsearch_const(vector, key, compar);
}

const void *bds_vector_lsearch_const(const struct bds_vector *vector, const void *key,
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

void *bds_vector_bsearch(struct bds_vector *vector, const void *key, int (*compar)(const void *a, const void *b))
{
        return (void *)bsearch(key, vector->v, vector->n_elem, vector->elem_len, compar);
}

const void *bds_vector_bsearch_const(const struct bds_vector *vector, const void *key,
                                     int (*compar)(const void *a, const void *b))
{
        return bsearch(key, vector->v, vector->n_elem, vector->elem_len, compar);
}

void bds_vector_qsort(struct bds_vector *vector, int (*compar)(const void *, const void *))
{
        qsort(vector->v, vector->n_elem, vector->elem_len, compar);
}
