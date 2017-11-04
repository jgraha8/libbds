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
};

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

        free(node);

        return 0;
}

static void destroy_list(struct list_node *node, void (*elem_dtor)(void *))
{
        if (node == NULL)
                return;

        if (elem_dtor)
                elem_dtor(node->v);

        destroy_list(node->next, elem_dtor);

        free(node);
}

void bds_list_ctor(struct bds_list *list, size_t elem_len, void (*elem_dtor)(void *))
{
        memset(list, 0, sizeof(*list));

        list->elem_len = elem_len;
	list->elem_dtor = elem_dtor;
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
        struct list_node *node = xalloc(sizeof(*node) + elem_len);

        node->v = (void *)node + sizeof(*node);
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

int bds_list_insert_sort(struct bds_list *list, const void *v, int (*compar)(const void *, const void *))
{
        struct list_node **node = &list->head;
	struct list_node *prev = NULL;
	
	while( *node && compar(v, (*node)->v) > 0 ) {
		prev = *node;
		node = &(*node)->next;
	}
	
	struct list_node *new_node = alloc_node(list->elem_len, v);

	new_node->next = *node;
	*node = new_node;

	if( prev ) {
		prev->next = *node;
	}

        return 0;
}


struct test {
        float f;
        int i;
};

struct super_test {
	struct test *test;
};

struct super_test *alloc_super_test( float f, int i )
{
	struct super_test *s = malloc(sizeof(*s));
	s->test = malloc(sizeof(*s->test));

	s->test->f = f;
	s->test->i = i;

	return s;
}
	

void print_node_int(const struct list_node *node) { printf("%d", *(int *)node->v); }

void print_test(const struct list_node *node)
{
        const struct test *t = node->v;
        printf("{ %d, %f }", t->i, t->f);
}

void print_super_test(const struct list_node *node)
{
	const struct test *t = (*(struct super_test **)node->v)->test;
	printf("{ %d, %f }", t->i, t->f);	
}

int compar_int(const void *a, const void *b) { return (*(int *)a - *(int *)b); }

int compar_test(const void *a, const void *b) { return (((struct test *)a)->i - ((struct test *)b)->i); }

int compar_super_test(const void *a, const void *b) {
	return ((*(struct super_test **)a)->test->i - (*(struct super_test **)b)->test->i);
}

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

void test_struct_dtor(void *v)
{
	struct test *t = v;
	memset(t, 0, sizeof(*t));
	return;
}

void super_test_free(void *v)
{
	struct super_test *s = *(struct super_test **)v;

	test_struct_dtor( s->test );
	free( s->test );
	free(s);

	return;
}

#define COMPAR compar_super_test
#define PRINT_NODE print_super_test
#define ELEM_DTOR super_test_free

//#define print_list(a,b)

int main(void)
{

        struct bds_list *list = bds_list_alloc(sizeof(struct super_test *), ELEM_DTOR);

	struct super_test *s;	
        //struct test i = { 42, 43.0 };
	struct super_test *key = alloc_super_test( 43, 42 );
	
        bds_list_insert_sort(list, &key, COMPAR);

        print_list(list, PRINT_NODE);

	s = alloc_super_test(86, 84);	
        assert( bds_list_insert_sort(list, &s, COMPAR) == 0 );
        print_list(list, PRINT_NODE);
	
	s = alloc_super_test(22,21);
        assert( bds_list_insert_sort(list, &s, COMPAR) == 0 );
        print_list(list, PRINT_NODE);



        assert( bds_list_remove(list, &key, COMPAR) == 0 );
        print_list(list, PRINT_NODE);

        bds_list_free(&list);

        return 0;
}
