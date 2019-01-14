/**
 * @file
 * @brief Heap data structure module definitions
 *
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
#ifndef __BDS_HEAP_H__
#define __BDS_HEAP_H__

#define BDS_HEAP_MAX (1 << 0)
#define BDS_HEAP_MIN (1 << 1)

#ifdef __cplusplus
extern "C" {
#endif

struct bds_heap;

struct bds_heap *bds_heap_alloc(int heap_type, int num_alloc, size_t elem_len,
                                int (*compar)(const void *, const void *), void *data);

void bds_heap_free(struct bds_heap **heap);

void *bds_heap_assign(struct bds_heap *heap, const void *data, int num_elem);
const void *bds_heap_ptr(const struct bds_heap *heap);
size_t bds_heap_size(const struct bds_heap *heap);

void bds_heap_heapify(struct bds_heap *heap, int node);
void bds_heap_reparent(struct bds_heap *heap, int node);
void bds_heap_change_key(struct bds_heap *heap, int node, const void *val);
void bds_heap_build(struct bds_heap *heap);
int bds_heap_pop(struct bds_heap *heap, void *val);
int bds_heap_insert(struct bds_heap *heap, const void *val);

int bds_heap_get(const struct bds_heap *heap, int node, void *val);

void bds_heapsort(void *data, int num_elem, size_t elem_len, int (*compar)(const void *, const void *));

#ifdef __cplusplus
}
#endif

#ifdef BDS_NAMESPACE
#define HEAP_MAX BDS_HEAP_MAX
#define HEAP_MIN BDS_HEAP_MIN

typedef struct bds_heap heap_t;

#define heap_alloc bds_heap_alloc
#define heap_freee bds_heap_free
#define heap_assign bds_heap_assign
#define heap_ptr bds_heap_ptr
#define heap_size bds_heap_size
#define heap_heapify bds_heap_heapify
#define heap_reparent bds_heap_reparent
#define heap_change_key bds_heap_change_key
#define heap_build bds_heap_build
#define heap_pop bds_heap_pop
#define heap_insert bds_heap_insert
#define heap_get bds_heap_get
#define heapsort bds_heapsort
#endif // BDS_NAMESPACE

#endif // __BDS_HEAP_H__
