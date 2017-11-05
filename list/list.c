#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memutil.h"
//#include <libbds/bds_stack.h>
#include "bds_list.h"

static struct list_node **list_end(struct list_node **node);
static void destroy_list_nodes(struct list_node **head, void (*object_dtor)(void *));
static struct list_node *alloc_node(size_t object_len, const void *v);
static void free_node(struct list_node **node, void (*object_dtor)(void *));
static struct list_node **find_node_ptr(struct list_node **node_ptr, const void *key,
                                        int (*object_compar)(const void *, const void *));

void bds_list_ctor(struct bds_list *list, size_t object_len, int (*object_compar)(const void *, const void *),
                   void (*object_dtor)(void *))
{
        memset(list, 0, sizeof(*list));

        list->object_len    = object_len;
        list->object_compar = object_compar;
        list->object_dtor   = object_dtor;
}

void bds_list_dtor(struct bds_list *list)
{
        destroy_list_nodes(&list->head, list->object_dtor);
        memset(list, 0, sizeof(*list));
}

struct bds_list *bds_list_alloc(size_t object_len, int (*object_compar)(const void *, const void *),
                                void (*object_dtor)(void *))
{
        struct bds_list *list = malloc(sizeof(*list));
        bds_list_ctor(list, object_len, object_compar, object_dtor);

        return list;
};

void bds_list_free(struct bds_list **list)
{
        if (*list == NULL)
                return;

        bds_list_dtor(*list);
        free(*list);
        *list = NULL;
}

void bds_list_append(struct bds_list *list, const void *v) { *list_end(&list->head) = alloc_node(list->object_len, v); }

int bds_list_remove(struct bds_list *list, const void *key)
{
        struct list_node **node_ptr = find_node_ptr(&list->head, key, list->object_compar);

        // Did not find the key
        if (*node_ptr == NULL)
                return 1;

        struct list_node *rm_node = *node_ptr;
        *node_ptr                 = (*node_ptr)->next;
        free_node(&rm_node, list->object_dtor);

        return 0;
}

int bds_list_insert_after(struct bds_list *list, const void *v, const void *key)
{
        struct list_node **node_ptr = find_node_ptr(&list->head, key, list->object_compar);

        // Did not find the key
        if (*node_ptr == NULL)
                return 1;

        node_ptr = &(*node_ptr)->next;

        struct list_node *new_node = alloc_node(list->object_len, v);
        new_node->next             = *node_ptr;
        *node_ptr                  = new_node;

        return 0;
}

int bds_list_insert_before(struct bds_list *list, const void *v, const void *key)
{
        struct list_node **node_ptr = find_node_ptr(&list->head, key, list->object_compar);

        // Did not find the key
        if (*node_ptr == NULL)
                return 1;

        struct list_node *new_node = alloc_node(list->object_len, v);
        new_node->next             = *node_ptr;
        *node_ptr                  = new_node;

        return 0;
}

int bds_list_insert_sort(struct bds_list *list, const void *v)
{
        struct list_node **node_ptr = &list->head;

        while (*node_ptr && list->object_compar(v, (*node_ptr)->v) > 0) {
                node_ptr = &(*node_ptr)->next;
        }

        struct list_node *new_node = alloc_node(list->object_len, v);
        new_node->next             = *node_ptr;
        *node_ptr                  = new_node;

        return 0;
}

static struct list_node **list_end(struct list_node **node)
{
        while (*node) {
                node = &(*node)->next;
        }

        return node;
}

static void destroy_list_nodes(struct list_node **head, void (*object_dtor)(void *))
{
        struct list_node *node = *head;
        while (node) {
                struct list_node *next = node->next;
                free_node(&node, object_dtor);
                node = next;
        }
        *head = NULL;
}

static struct list_node *alloc_node(size_t object_len, const void *v)
{
        struct list_node *node = xalloc(sizeof(*node) + object_len);

        node->v = (void *)node + sizeof(*node);
        memcpy(node->v, v, object_len);

        return node;
}

static void free_node(struct list_node **node, void (*object_dtor)(void *))
{
        if (object_dtor)
                object_dtor((*node)->v);
        free(*node);
        *node = NULL;
}

static struct list_node **find_node_ptr(struct list_node **node_ptr, const void *key,
                                        int (*object_compar)(const void *, const void *))
{
        while (*node_ptr && object_compar((*node_ptr)->v, key) != 0) {
                node_ptr = &(*node_ptr)->next;
        }

        return node_ptr;
}

#ifdef __cplusplus
}
#endif
