/*
 * Copyright (C) 2017-2018 Jason Graham <jgraham@compukix.net>
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
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <libbds/bds_heap.h>

#include "heap/heap_util.h"
#include "memutil.h"

#define MAX(a, b) (a > b ? a : b)

struct bds_heap {
        void *data;
	void *tmp_data;
        int (*compar)(const void *, const void *);
        void (*heapify)(struct bds_heap *, int);
        void (*reparent)(struct bds_heap *, int);
        void (*change_key)(struct bds_heap *, int, const void *);
        size_t elem_len;
        int num_elem;
        int num_alloc;
        int heap_type;
        bool data_owner;
};

static void max_heapify(struct bds_heap *heap, int node)
{
        int left  = LEFT(node);
        int right = RIGHT(node);

        int child = -1;

	if( left < heap->num_elem && right < heap->num_elem ) {
		/* Find the largest child node */
		if( heap->compar(PTR(heap, left), PTR(heap, right)) >= 0) {
			child = left;
		} else {
			child = right;
		}
	} else if( left < heap->num_elem ) {
		child = left;
	} else if( right < heap->num_elem ) {
		child = right;
	} else { /* Both left and right are out of bounds */
		return;
	}

	if( heap->compar(PTR(heap,node), PTR(heap,child)) < 0 ) {
                xmemswap(PTR(heap, child), PTR(heap, node), heap->elem_len, heap->tmp_data);
                max_heapify(heap, child);
        }
}

static void min_heapify(struct bds_heap *heap, int node)
{
        int left  = LEFT(node);
        int right = RIGHT(node);

        int child = -1;

	if( left < heap->num_elem && right < heap->num_elem ) {
		/* Find the smallest child node */
		if( heap->compar(PTR(heap, left), PTR(heap, right)) <= 0) {
			child = left;
		} else {
			child = right;
		}
	} else if( left < heap->num_elem ) {
		child = left;
	} else if( right < heap->num_elem ) {
		child = right;
	} else { /* Both left and right are out of bounds */
		return;
	}

	if( heap->compar(PTR(heap,node), PTR(heap,child)) > 0 ) {
                xmemswap(PTR(heap, child), PTR(heap, node), heap->elem_len, heap->tmp_data);
                min_heapify(heap, child);
        }
}

// Flow upwards
static void max_reparent(struct bds_heap *heap, int node)
{
        if (node <= 0)
                return;

        int parent = PARENT(node);

        if (heap->compar(PTR(heap, parent), PTR(heap, node)) < 0) {
                xmemswap(PTR(heap, parent), PTR(heap, node), heap->elem_len, heap->tmp_data);
                max_reparent(heap, parent);
        }
}

static void min_reparent(struct bds_heap *heap, int node)
{
        if (node <= 0)
                return;

        int parent = PARENT(node);

        if (heap->compar(PTR(heap, parent), PTR(heap, node)) > 0) {
                xmemswap(PTR(heap, parent), PTR(heap, node), heap->elem_len, heap->tmp_data);
                min_reparent(heap, parent);
        }
}

static void max_change_key(struct bds_heap *heap, int node, const void *val)
{
        int c = heap->compar(val, PTR(heap, node));

        if (c == 0)
                return;

        memcpy(PTR(heap, node), val, heap->elem_len);

        if (c < 0) {
                heap->heapify(heap, node);
        } else {
                heap->reparent(heap, node);
        }
}

static void min_change_key(struct bds_heap *heap, int node, const void *val)
{
        int c = heap->compar(val, PTR(heap, node));

        if (c == 0)
                return;

        memcpy(PTR(heap, node), val, heap->elem_len);

        if (c < 0) {
                heap->reparent(heap, node);
        } else {
                heap->heapify(heap, node);
        }
}

struct bds_heap *bds_heap_alloc(int heap_type, int num_alloc, size_t elem_len,
                                int (*compar)(const void *, const void *), void *data)
{
        struct bds_heap *heap = calloc(1, sizeof(*heap));
        assert(heap);

        heap->heap_type = heap_type;

        if (heap_type == BDS_HEAP_MAX) {
                heap->heapify    = max_heapify;
                heap->reparent   = max_reparent;
                heap->change_key = max_change_key;
        } else if (heap_type == BDS_HEAP_MIN) {
                heap->heapify    = min_heapify;
                heap->reparent   = min_reparent;
                heap->change_key = min_change_key;
        } else {
                free(heap);
                fprintf(stderr, "incorrect heap type\n");
                exit(EXIT_FAILURE);
        }

        heap->num_alloc = num_alloc;
        heap->elem_len  = elem_len;
        heap->compar    = compar;

        if (data) {
                heap->num_elem = num_alloc;
                heap->data     = data;
        } else {
                heap->data = calloc(num_alloc, elem_len);
                assert(heap->data);

                heap->data_owner = true;
        }
	heap->tmp_data = calloc(1, elem_len);

        return heap;
}

void bds_heap_free(struct bds_heap **heap)
{
        if (*heap == NULL)
                return;

        if ((*heap)->data_owner) {
                if ((*heap)->data)
                        free((*heap)->data);
        }
	free((*heap)->tmp_data);
        free(*heap);
        *heap = NULL;
}

void *bds_heap_assign(struct bds_heap *heap, const void *data, int num_elem)
{
        if (num_elem > heap->num_alloc) {
                if (heap->data_owner) {
                        size_t len_old = heap->num_alloc * heap->elem_len;

                        heap->num_elem = num_elem;
                        heap->data     = xrealloc(heap->data, len_old, heap->num_elem * heap->elem_len);

                } else {
                        return NULL;
                }
        }

        heap->num_elem = num_elem;
        memcpy(heap->data, data, num_elem * heap->elem_len);

        return heap->data;
}

const void *bds_heap_ptr(const struct bds_heap *heap)
{
        return heap->data;
}

size_t bds_heap_size(const struct bds_heap *heap)
{
        return heap->num_elem;
}

void bds_heap_heapify(struct bds_heap *heap, int node)
{
        heap->heapify(heap, node);
}
void bds_heap_reparent(struct bds_heap *heap, int node)
{
        heap->reparent(heap, node);
}
void bds_heap_change_key(struct bds_heap *heap, int node, const void *val)
{
        heap->change_key(heap, node, val);
}

void bds_heap_build(struct bds_heap *heap)
{
        for (int n = heap->num_elem / 2 - 1; n >= 0; --n) {
                heap->heapify(heap, n);
        }
}

int bds_heap_pop(struct bds_heap *heap, void *val)
{
        if (heap->num_elem == 0)
                return 1;

        memcpy(val, PTR(heap, 0), heap->elem_len);
        memcpy(PTR(heap, 0), PTR(heap, heap->num_elem - 1), heap->elem_len);

        --heap->num_elem;
        heap->heapify(heap, 0);

        return 0;
}
int bds_heap_insert(struct bds_heap *heap, const void *val)
{
        if (heap->num_elem == heap->num_alloc) {
                if (heap->data_owner) {
                        size_t len_old = heap->num_alloc * heap->elem_len;

                        heap->num_alloc <<= 1;
                        heap->data = xrealloc(heap->data, len_old, heap->num_alloc * heap->elem_len);
                } else {
                        return 1;
                }
        }

        ++heap->num_elem;
        memcpy(PTR(heap, heap->num_elem - 1), val, heap->elem_len);
        heap->reparent(heap, heap->num_elem - 1);

        return 0;
}

int bds_heap_get(const struct bds_heap *heap, int node, void *val)
{
        if (heap->num_elem == 0)
                return 1;

        memcpy(val, PTR(heap, node), heap->elem_len);
        return 0;
}

void bds_heapsort(void *data, int num_elem, size_t elem_len, int (*compar)(const void *, const void *))
{
        struct bds_heap *heap = bds_heap_alloc(BDS_HEAP_MAX, num_elem, elem_len, compar, data);

        bds_heap_build(heap);

        while (heap->num_elem > 1) {
                xmemswap(PTR(heap, 0), PTR(heap, heap->num_elem - 1), heap->elem_len, heap->tmp_data);
                --heap->num_elem;
                bds_heap_heapify(heap, 0);
        }

        bds_heap_free(&heap);
}
