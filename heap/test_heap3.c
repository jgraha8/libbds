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

#define BDS_NAMESPACE
#include <libbds/bds_heap.h>

#include "heap/heap_util.h"

#define N 20

int compar(const void *a, const void *b)
{
        return (*((const int *)a) - *((const int *)b));
}

int main(int argc, char **argv)
{
        int a[N];
	const int *a_ptr;

        srand(time(NULL));
        for (int i = 0; i < N; ++i) {
                a[i] = rand() % 100;
        }

        // struct bds_heap *heap = bds_heap_alloc(BDS_HEAP_MAX, N, sizeof(a[0]), compar, a);
        heap_t *heap = heap_alloc(HEAP_MIN, 1, sizeof(a[0]), compar, NULL);

	for( int i=0; i<N; ++i ) {
		heap_insert(heap, &a[i]);
	}
        assert(heap_size(heap) == N);

        heap_build(heap);

	a_ptr = heap_ptr(heap);

        for (int i = 1; i < N; ++i) {
                assert(a_ptr[PARENT(i)] <= a_ptr[i]);
        }

        return 0;
}
