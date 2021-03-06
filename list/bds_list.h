/**
 * @file
 * @brief Linked list data structure module definitions
 *
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

#ifndef __BDS_LIST_H__
#define __BDS_LIST_H__

#ifdef __cplusplus
extern "C" {
#endif

struct bds_list;
struct bds_list_node;

/* void bds_list_ctor(struct bds_list *list, size_t object_len, int (*object_compar)(const void *, const void *),
 */
/*                    void (*object_dtor)(void *)); */

/* void bds_list_dtor(struct bds_list *list); */

struct bds_list *bds_list_alloc(size_t object_len, void (*object_dtor)(void *));

void bds_list_free(struct bds_list **list);

// static inline struct bds_list_node *bds_list_begin(struct bds_list *list) { return list->head; }
// static inline struct bds_list_node *bds_list_end() { return NULL; }
// static inline struct bds_list_node *bds_list_iterate(struct bds_list_node *node) { return node->next; }

struct bds_list_node *bds_list_begin(struct bds_list *list);
static inline struct bds_list_node *bds_list_end() { return NULL; }
struct bds_list_node *bds_list_iterate(struct bds_list_node *node);

// static inline void *bds_list_object(struct bds_list_node *node) { return node->object; }
void *bds_list_object(struct bds_list_node *node);

void bds_list_print(const struct bds_list *list, void (*print_object)(const void *));

void bds_list_append(struct bds_list *list, const void *object);

int bds_list_remove(struct bds_list *list, const void *key, int (*compar)(const void *, const void *));

int bds_list_insert_after(struct bds_list *list, const void *object, const void *key,
                          int (*compar)(const void *, const void *));

int bds_list_insert_before(struct bds_list *list, const void *object, const void *key,
                           int (*compar)(const void *, const void *));

int bds_list_insert_sort(struct bds_list *list, const void *object, int (*compar)(const void *, const void *));

void *bds_list_lsearch(struct bds_list *list, const void *key, int (*compar)(const void *, const void *));

#ifdef __cplusplus
}
#endif

#ifdef BDS_NAMESPACE
typedef struct bds_list list_t;
typedef struct bds_list_node list_node_t;
#define list_alloc bds_list_alloc
#define list_free bds_list_free
#define list_begin bds_list_begin
#define list_end bds_list_end
#define list_iterate bds_list_iterate
#define list_object bds_list_object
#define list_print bds_list_print
#define list_append bds_list_append
#define list_remove bds_list_remove
#define list_insert_after bds_list_insert_after
#define list_insert_before bds_list_insert_before
#define list_insert_sort bds_list_insert_sort
#define list_lsearch bds_list_lsearch
#endif // BDS_NAMESPACE

#endif // __BDS_LIST_H__
