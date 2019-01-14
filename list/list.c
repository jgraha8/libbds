/**
 * @file
 * @brief Linked list data structure module
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
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memutil.h"
#include <libbds/bds_list.h>

struct bds_list_node {
        void *object;
        struct bds_list_node *next;
};

struct bds_list {
        size_t object_len;
        void (*object_dtor)(void *);
        struct bds_list_node *head;
};

static struct bds_list_node **list_end(struct bds_list_node **node);
static void destroy_list_nodes(struct bds_list_node **head, void (*object_dtor)(void *));
static struct bds_list_node *alloc_node(size_t object_len, const void *object);
static void free_node(struct bds_list_node **node, void (*object_dtor)(void *));
static struct bds_list_node **find_node_ptr(struct bds_list_node **node_ptr, const void *key,
                                            int (*compar)(const void *, const void *));

static void __list_ctor(struct bds_list *list, size_t object_len, void (*object_dtor)(void *))
{
        memset(list, 0, sizeof(*list));

        list->object_len  = object_len;
        list->object_dtor = object_dtor;
}

static void __list_dtor(struct bds_list *list)
{
        destroy_list_nodes(&list->head, list->object_dtor);
        memset(list, 0, sizeof(*list));
}

struct bds_list *bds_list_alloc(size_t object_len, void (*object_dtor)(void *))
{
        struct bds_list *list = malloc(sizeof(*list));
        __list_ctor(list, object_len, object_dtor);

        return list;
};

void bds_list_free(struct bds_list **list)
{
        if (*list == NULL)
                return;

        __list_dtor(*list);
        free(*list);
        *list = NULL;
}

struct bds_list_node *bds_list_begin(struct bds_list *list) { return list->head; }

struct bds_list_node *bds_list_iterate(struct bds_list_node *node) { return node->next; }

void *bds_list_object(struct bds_list_node *node) { return node->object; }

void bds_list_print(const struct bds_list *list, void (*print_object)(const void *))
{
        struct bds_list_node *node = bds_list_begin((struct bds_list *)list);
        bool first_node            = true;

        for (; node != bds_list_end(); node = bds_list_iterate(node)) {
                if (!first_node) {
                        printf(", ");
                }
                first_node = false;
                print_object(bds_list_object(node));
        }
        printf("\n");
}

void bds_list_append(struct bds_list *list, const void *object)
{
        *list_end(&list->head) = alloc_node(list->object_len, object);
}

int bds_list_remove(struct bds_list *list, const void *key, int (*compar)(const void *, const void *))
{
        struct bds_list_node **node_ptr = find_node_ptr(&list->head, key, compar);

        // Did not find the key
        if (*node_ptr == NULL)
                return 1;

        struct bds_list_node *rm_node = *node_ptr;
        *node_ptr                     = (*node_ptr)->next;
        free_node(&rm_node, list->object_dtor);

        return 0;
}

int bds_list_insert_after(struct bds_list *list, const void *object, const void *key,
                          int (*compar)(const void *, const void *))
{
        struct bds_list_node **node_ptr = find_node_ptr(&list->head, key, compar);

        // Did not find the key
        if (*node_ptr == NULL)
                return 1;

        node_ptr = &(*node_ptr)->next;

        struct bds_list_node *new_node = alloc_node(list->object_len, object);
        new_node->next                 = *node_ptr;
        *node_ptr                      = new_node;

        return 0;
}

int bds_list_insert_before(struct bds_list *list, const void *object, const void *key,
                           int (*compar)(const void *, const void *))
{
        struct bds_list_node **node_ptr = find_node_ptr(&list->head, key, compar);

        // Did not find the key
        if (*node_ptr == NULL)
                return 1;

        struct bds_list_node *new_node = alloc_node(list->object_len, object);
        new_node->next                 = *node_ptr;
        *node_ptr                      = new_node;

        return 0;
}

int bds_list_insert_sort(struct bds_list *list, const void *object, int (*compar)(const void *, const void *))
{
        struct bds_list_node **node_ptr = &list->head;

        while (*node_ptr && compar(object, (*node_ptr)->object) > 0) {
                node_ptr = &(*node_ptr)->next;
        }

        struct bds_list_node *new_node = alloc_node(list->object_len, object);
        new_node->next                 = *node_ptr;
        *node_ptr                      = new_node;

        return 0;
}

void *bds_list_lsearch(struct bds_list *list, const void *key, int (*compar)(const void *, const void *))
{
        struct bds_list_node **node_ptr = find_node_ptr(&list->head, key, compar);
        if (*node_ptr == NULL)
                return NULL;

        return (*node_ptr)->object;
}

static struct bds_list_node **list_end(struct bds_list_node **node)
{
        while (*node) {
                node = &(*node)->next;
        }
        return node;
}

static void destroy_list_nodes(struct bds_list_node **head, void (*object_dtor)(void *))
{
        struct bds_list_node *node = *head;
        while (node) {
                struct bds_list_node *next = node->next;
                free_node(&node, object_dtor);
                node = next;
        }
        *head = NULL;
}

static struct bds_list_node *alloc_node(size_t object_len, const void *object)
{
        struct bds_list_node *node = xalloc(sizeof(*node) + object_len);

        node->object = (void *)node + sizeof(*node);
        memcpy(node->object, object, object_len);

        return node;
}

static void free_node(struct bds_list_node **node, void (*object_dtor)(void *))
{
        if (object_dtor)
                object_dtor((*node)->object);
        free(*node);
        *node = NULL;
}

static struct bds_list_node **find_node_ptr(struct bds_list_node **node_ptr, const void *key,
                                            int (*compar)(const void *, const void *))
{
        while (*node_ptr && compar((*node_ptr)->object, key) != 0) {
                node_ptr = &(*node_ptr)->next;
        }

        return node_ptr;
}

#ifdef __cplusplus
}
#endif
