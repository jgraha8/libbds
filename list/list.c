#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "memutil.h"
//#include <libbds/bds_stack.h>

static void *xalloc(size_t len)
{
        void *v = malloc(len);
        assert(v);
        memset(v, 0, len);
        return v;
}

struct list_node;

struct list_node {
        void *v;
        struct list_node *next;
} list_node;

struct bds_list {
        size_t elem_len;
        void (*elem_dtor)(void *);
        struct list_node *head;
};

static int free_list_node(struct list_node *node, struct list_node *prev, void (*elem_dtor)(void *v))
{
        if (node == NULL)
                return 1;

        if (prev)
                prev->next = node->next;

        if (elem_dtor)
                elem_dtor(node->v);

        free(node->v);
        free(node);

        return 0;
}

static void destroy_list(struct list_node *node, void (*elem_dtor)(void *))
{
        if (node == NULL)
                return;

        if (elem_dtor)
                elem_dtor(node->v);
        free(node->v);

        destroy_list(node->next, elem_dtor);

        free(node);
}

void bds_list_ctor(struct bds_list *list, size_t elem_len, void (*elem_dtor)(void *))
{
        memset(list, 0, sizeof(*list));

        list->elem_len = elem_len;
}

void bds_list_dtor(struct bds_list *list) { destroy_list(list->head, list->elem_dtor); }

struct bds_list *bds_list_alloc(size_t elem_len, void (*elem_dtor)(void *))
{
        struct bds_list *list = malloc(sizeof(*list));
        bds_list_ctor(list, elem_len, elem_dtor);

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

struct list_node *last_node(struct list_node *node)
{
        assert(node);

        if (node->next == NULL)
                return node;

        return last_node(node->next);
}

struct list_node *alloc_node(size_t elem_len, const void *v)
{
        struct list_node *node = xalloc(sizeof(*node));

        node->v = malloc(elem_len);
        memcpy(node->v, v, elem_len);

        return node;
}

void bds_list_append(struct bds_list *list, const void *v)
{
        struct list_node **next = NULL;
        if (list->head == NULL) {
                next = &list->head;
        } else {
                next = &last_node(list->head)->next;
        }

        *next = alloc_node(list->elem_len, v);
}

static void find_node(struct list_node **node, struct list_node **prev, const void *key,
                      int (*compar)(const void *, const void *))
{
        if (*node == NULL)
                return;

        if (compar((*node)->v, key) == 0)
                return;

        if (prev)
                *prev = *node;

        *node = (*node)->next;

        find_node(node, prev, key, compar);

        return;
}

int bds_list_remove(struct bds_list *list, const void *key, int (*compar)(const void *, const void *))
{
        struct list_node *prev = NULL, *node = list->head;

        find_node(&node, &prev, key, compar);

        return free_list_node(node, prev, list->elem_dtor);
}

int bds_list_insert_after(struct bds_list *list, const void *v, const void *key,
                          int (*compar)(const void *, const void *))
{
        struct list_node *node = list->head;

        find_node(&node, NULL, key, compar);

        if (node == NULL)
                return 1;

        struct list_node *new_node = alloc_node(list->elem_len, v);
        new_node->next             = node->next;
        node->next                 = new_node;

        return 0;
}

int bds_list_insert_before(struct bds_list *list, const void *v, const void *key,
                           int (*compar)(const void *, const void *))
{
        struct list_node *prev = NULL, *node = list->head;

        find_node(&node, &prev, key, compar);

        if (node == NULL)
                return 1;

        struct list_node *new_node = alloc_node(list->elem_len, v);

        if (prev) {
                prev->next = new_node;
        } else {
                // Reset head
                assert(node == list->head);
                list->head = new_node;
        }
        new_node->next = node;

        return 0;
}

struct test {
        float f;
        int i;
};

void print_node_int(const struct list_node *node) { printf("%d", *(int *)node->v); }

void print_node_s(const struct list_node *node)
{
        const struct test *t = node->v;
        printf("{ %d, %f }", t->i, t->f);
}

int compar_int(const void *a, const void *b) { return (*(int *)b - *(int *)a); }

int compar_s(const void *a, const void *b) { return (((struct test *)b)->i - ((struct test *)a)->i); }

void print_list(const struct bds_list *list, void (*print_node)(const struct list_node *))
{
        const struct list_node *node = list->head;

        int i = 0;
        while (node) {
                if (i != 0) {
                        printf(", ");
                }
                i = 1;
                print_node(node);
                node = node->next;
        }
        printf("\n");
}

#define COMPAR compar_s
#define PRINT_NODE print_node_s

int main(void)
{

        struct bds_list *list = bds_list_alloc(sizeof(struct test), NULL);

        struct test i = { 42, 43.0 };
        bds_list_append(list, &i);

        print_list(list, PRINT_NODE);

        struct test j = { 21, 22.0 };
        assert( bds_list_insert_before(list, &j, &i, COMPAR) == 0 );
        print_list(list, PRINT_NODE);

        j.i = 84;
	j.f = 85.0;
	
        assert( bds_list_insert_after(list, &j, &i, COMPAR) == 0 );
        print_list(list, PRINT_NODE);

        assert( bds_list_remove(list, &i, COMPAR) == 0 );
        print_list(list, PRINT_NODE);

        bds_list_free(&list);

        return 0;
}
