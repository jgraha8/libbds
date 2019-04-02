/*
 * Copyright (C) 2018 Jason Graham <jgraham@compukix.net>
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
#include <time.h>
#include <limits.h>

#define BDS_NAMESPACE
#include <libbds/bds_heap.h>

#include "heap/heap_util.h"

#define N 20

struct test {
	double v;
	int i;
};

int compar(const void *a, const void *b)
{
	const struct test *__a = (const struct test *)a;
	const struct test *__b = (const struct test *)b;

	if( __a->v < __b->v ) 
		return -1;
	if( __a->v > __b->v )
		return 1;
	return 0;
}


int main(int argc, char **argv)
{
        struct test a[N];
	const struct test *a_ptr;

        srand(time(NULL));
        for (int i = 0; i < N; ++i) {
                a[i].v = 0.123 * (rand() % 100);
		a[i].i = i;
        }

        heap_t *heap = heap_alloc(HEAP_MIN, 1, sizeof(a[0]), compar, NULL);

	for( int i=0; i<N; ++i ) {
		heap_insert(heap, &a[i]);
	}
        assert(heap_size(heap) == N);

	a_ptr = heap_ptr(heap);
	
        for (int i = 1; i < N; ++i) {
                assert(a_ptr[PARENT(i)].v <= a_ptr[i].v);
        }

	double v_prev = -1.0e32;
	struct test v;
	while(bds_heap_pop(heap, &v) == 0 ) {
		assert(v.v >= v_prev);
		v_prev = v.v;
	}

        return 0;
}
