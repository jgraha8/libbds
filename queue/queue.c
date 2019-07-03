/*
 * Copyright (C) 2017 Jason Graham <jgraham@compukix.net>
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
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memutil.h"
#include <libbds/bds_queue.h>

/* static inline double log2( double x ) */
/* { */
/* 	return ( log(x) / log(2.0) ); */
/* } */

inline static void __queue_incr_front(struct bds_queue *queue)
{
        queue->front = BDS_MOD(queue->front + 1, queue->n_alloc);
}

inline static size_t __queue_end(const struct bds_queue *queue)
{
        assert(queue->n_alloc > 0);
        return BDS_MOD(bds_queue_back(queue) + 1, queue->n_alloc);
}

inline static const void *__queue_endptr(const struct bds_queue *queue)
{
        return (const void *)((char *)queue->v + __queue_end(queue) * queue->elem_len);
}

void bds_queue_ctor(struct bds_queue *queue, size_t n_alloc, size_t elem_len, void (*elem_dtor)(void *))
{
        memset(queue, 0, sizeof(*queue));

#ifdef BDS_USE_OPTIMIZED_MOD
        queue->n_alloc = (1UL << (unsigned long)ceil(log2(n_alloc)));
#else
        queue->n_alloc = n_alloc;
#endif
        queue->elem_len = elem_len;

        queue->v         = xalloc(n_alloc * elem_len);
        queue->elem_dtor = elem_dtor;
}

void bds_queue_dtor(struct bds_queue *queue)
{
        if (queue->v != NULL) {
                bds_queue_clear(queue);
                free(queue->v);
        }
        memset(queue, 0, sizeof(*queue));
}

struct bds_queue *bds_queue_alloc(size_t n_alloc, size_t elem_len, void (*elem_dtor)(void *))
{
        struct bds_queue *queue = xalloc(sizeof(*queue));
        bds_queue_ctor(queue, n_alloc, elem_len, elem_dtor);
        return queue;
}

void bds_queue_free(struct bds_queue **queue)
{
        if (*queue == NULL)
                return;

        bds_queue_dtor(*queue);
        free(*queue);
        *queue = NULL;
}

void bds_queue_resize(struct bds_queue *queue)
{
        const size_t n_alloc  = queue->n_alloc << 1; // Double the size (need overflow check here)
        const size_t elem_len = queue->elem_len;

        queue->v = xrealloc(queue->v, queue->n_alloc * elem_len, n_alloc * elem_len);

        bds_queue_linearize(queue);
        queue->n_alloc = n_alloc;
}

int bds_queue_push(struct bds_queue *queue, const void *v)
{
        if (bds_queue_isfull(queue)) {
                assert(__queue_end(queue) == queue->front);
                if (queue->auto_resize) {
                        bds_queue_resize(queue);
                } else {
                        return 1;
                }
        }

        // Copy to the vacant location
        memcpy((void *)__queue_endptr(queue), v, queue->elem_len);
        queue->n_elem++;

        return 0;
}

void *bds_queue_pop(struct bds_queue *queue, void *v)
{
        void *v_front = (void *)bds_queue_frontptr(queue);
        if (v_front == NULL)
                return NULL;

        if (v != NULL)
                memcpy(v, v_front, queue->elem_len);

        __queue_incr_front(queue);
        queue->n_elem--;

        return v_front;
}

void bds_queue_clear(struct bds_queue *queue)
{
        if (queue->elem_dtor != NULL) {
                while (!bds_queue_isempty(queue)) {
                        queue->elem_dtor(bds_queue_pop(queue, NULL));
                }
        }
        queue->front  = 0;
        queue->n_elem = 0;
}

void bds_queue_clear_nfront(struct bds_queue *queue, size_t n_clear)
{
        if (bds_queue_size(queue) < n_clear) {
                printf("warning: bds_queue_clear_nfront: n_clear larger than size of queue\n");
                bds_queue_clear(queue);
                return;
        }

        if (queue->elem_dtor != NULL) {
                size_t n;
                for (n = 0; n < n_clear; ++n) {
                        queue->elem_dtor(bds_queue_pop(queue, NULL));
                }
        } else {
                queue->front = BDS_MOD(queue->front + n_clear, queue->n_alloc);
                queue->n_elem -= n_clear;
        }
}

void bds_queue_linearize(struct bds_queue *queue)
{

        const size_t front = queue->front;
        const size_t back  = bds_queue_back(queue);

        if (BDS_QUEUE_ISLINEAR(front, back))
                return;

        const size_t blk_len = (queue->n_alloc - front) * queue->elem_len;
        void *buffer         = xalloc(blk_len);

        // Store the trailing segment of the data vector into a buffer
        memcpy(buffer, bds_queue_frontptr(queue), blk_len);
        // Shift the remaining elements to make room for the front of the data vector
        memmove((char *)queue->v + blk_len, queue->v, front * queue->elem_len);
        // Copy the front data vector block to the front
        memcpy(queue->v, buffer, blk_len);
        free(buffer);

        queue->front = 0;
}

void *bds_queue_lsearch(struct bds_queue *queue, const void *key, int (*compar)(const void *, const void *))
{
        return (void *)bds_queue_lsearch_const(queue, key, compar);
}

const void *bds_queue_lsearch_const(const struct bds_queue *queue, const void *key,
                                    int (*compar)(const void *, const void *))
{
        size_t i, j;
        const void *v;

        j = queue->front;
        for (i = 0; i < queue->n_elem; ++i) {
                v = (char *)queue->v + j * queue->elem_len;
                if (compar(key, v) == 0)
                        return v;
                j = BDS_MOD(j + 1, queue->n_alloc);
        }
        return NULL;
}
