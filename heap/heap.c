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
        int (*compar)(const void *, const void *);
        void (*__heapify)(struct bds_heap *, int);
        void (*__reparent)(struct bds_heap *, int);
        void (*__change_key)(struct bds_heap *, int, const void *);
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

        if (left >= heap->num_elem && right >= heap->num_elem)
                return;

        int c       = -INT_MAX;
        int largest = -1;

        if (left < heap->num_elem) {
                int __c = heap->compar(PTR(heap, left), PTR(heap, node));
                if (__c > 0 && __c > c) {
                        c       = __c;
                        largest = left;
                }
        }
        if (right < heap->num_elem) {
                int __c = heap->compar(PTR(heap, right), PTR(heap, node));
                if (__c > 0 && __c > c) {
                        c       = __c;
                        largest = right;
                }
        }

        if (largest > 0) {
                char tmp[heap->elem_len];
                xmemswap(PTR(heap, largest), PTR(heap, node), heap->elem_len, tmp);
                max_heapify(heap, largest);
        }
}

static void min_heapify(struct bds_heap *heap, int node)
{
        int left  = LEFT(node);
        int right = RIGHT(node);

        if (left >= heap->num_elem && right >= heap->num_elem)
                return;

        int c        = INT_MAX;
        int smallest = -1;

        if (left < heap->num_elem) {
                int __c = heap->compar(PTR(heap, left), PTR(heap, node));
                if (__c < 0 && __c < c) {
                        c        = __c;
                        smallest = left;
                }
        }
        if (right < heap->num_elem) {
                int __c = heap->compar(PTR(heap, right), PTR(heap, node));
                if (__c < 0 && __c < c) {
                        c        = __c;
                        smallest = right;
                }
        }

        if (smallest > 0) {
                char tmp[heap->elem_len];
                xmemswap(PTR(heap, smallest), PTR(heap, node), heap->elem_len, tmp);
                min_heapify(heap, smallest);
        }
}

// Flow upwards
static void max_reparent(struct bds_heap *heap, int node)
{
        if (node <= 0)
                return;

        int parent = PARENT(node);

        if (heap->compar(PTR(heap, parent), PTR(heap, node)) < 0) {
                char tmp[heap->elem_len];
                xmemswap(PTR(heap, parent), PTR(heap, node), heap->elem_len, tmp);
                max_reparent(heap, parent);
        }
}

static void min_reparent(struct bds_heap *heap, int node)
{
        if (node <= 0)
                return;

        int parent = PARENT(node);

        if (heap->compar(PTR(heap, parent), PTR(heap, node)) > 0) {
                char tmp[heap->elem_len];
                xmemswap(PTR(heap, parent), PTR(heap, node), heap->elem_len, tmp);
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
                heap->__heapify(heap, node);
        } else {
                heap->__reparent(heap, node);
        }
}

static void min_change_key(struct bds_heap *heap, int node, const void *val)
{
        int c = heap->compar(val, PTR(heap, node));

        if (c == 0)
                return;

        memcpy(PTR(heap, node), val, heap->elem_len);

        if (c < 0) {
                heap->__reparent(heap, node);
        } else {
                heap->__heapify(heap, node);
        }
}

struct bds_heap *bds_heap_alloc(int heap_type, int num_alloc, size_t elem_len,
                                int (*compar)(const void *, const void *), void *data)
{
        struct bds_heap *heap = calloc(1, sizeof(*heap));
        assert(heap);

        heap->heap_type = heap_type;

        if (heap_type == BDS_HEAP_MAX) {
                heap->__heapify    = max_heapify;
                heap->__reparent   = max_reparent;
                heap->__change_key = max_change_key;
        } else if (heap_type == BDS_HEAP_MIN) {
                heap->__heapify    = min_heapify;
                heap->__reparent   = min_reparent;
                heap->__change_key = min_change_key;
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
        heap->__heapify(heap, node);
}
void bds_heap_reparent(struct bds_heap *heap, int node)
{
        heap->__reparent(heap, node);
}
void bds_heap_change_key(struct bds_heap *heap, int node, const void *val)
{
        heap->__change_key(heap, node, val);
}

void bds_heap_build(struct bds_heap *heap)
{
        for (int n = heap->num_elem / 2 - 1; n >= 0; --n) {
                heap->__heapify(heap, n);
        }
}

int bds_heap_pop(struct bds_heap *heap, void *val)
{
        if (heap->num_elem == 0)
                return 1;

        memcpy(val, PTR(heap, 0), heap->elem_len);
        memcpy(PTR(heap, 0), PTR(heap, heap->num_elem - 1), heap->elem_len);

        --heap->num_elem;
        heap->__heapify(heap, 0);

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
        heap->__reparent(heap, heap->num_elem - 1);

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
                char tmp[heap->elem_len];
                xmemswap(PTR(heap, 0), PTR(heap, heap->num_elem - 1), heap->elem_len, tmp);
                --heap->num_elem;
                bds_heap_heapify(heap, 0);
        }

        bds_heap_free(&heap);
}
