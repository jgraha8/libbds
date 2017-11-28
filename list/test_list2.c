#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libbds/bds_list.h>
#include <libbds/bds_version.h>

struct test {
        float f;
        int i;
};

struct super_test {
        struct test *test;
};

struct super_test create_super_test(float f, int i)
{
        struct super_test s;
        s.test = malloc(sizeof(*s.test));

        s.test->f = f;
        s.test->i = i;

        return s;
}

struct super_test *alloc_super_test(float f, int i)
{
        struct super_test *s = (struct super_test *)malloc(sizeof(*s));
        s->test              = (struct test *)malloc(sizeof(*s->test));

        s->test->f = f;
        s->test->i = i;

        return s;
}

void print_super_test(const void *object)
{
        const struct test *t = ((struct super_test *)object)->test;
        printf("{ %d, %f }", t->i, t->f);
}

int compar_super_test(const void *a, const void *b)
{
        return (((struct super_test *)a)->test->i - ((struct super_test *)b)->test->i);
}

int compar_super_test_addr(const void *a, const void *b)
{
        return (((struct super_test *)a)->test - ((struct super_test *)b)->test);
}
void test_struct_dtor(void *object)
{
        struct test *t = (struct test *)object;
        memset(t, 0, sizeof(*t));
        return;
}

void destroy_super_test(void *object)
{
        struct super_test *s = (struct super_test *)object;

        test_struct_dtor(s->test);
        free(s->test);

        return;

}

void super_test_free(void *object)
{
        struct super_test *s = (struct super_test *)object;

	destroy_super_test(s);
        free(s);

        return;
}

#define PRINT_NODE print_super_test
#define OBJECT_COMPAR compar_super_test
#define OBJECT_COMPAR_ADDR compar_super_test_addr
#define OBJECT_DTOR destroy_super_test

//#define print_list(a, b)

int main(void)
{

        struct bds_list *list = bds_list_alloc(sizeof(struct super_test), OBJECT_DTOR);

        struct super_test s[5];

        printf("bds_version = %s\n", bds_version);
        // struct test i = { 42, 43.0 };

        s[1] = create_super_test(86, 84);
        assert(bds_list_insert_sort(list, &s[1], OBJECT_COMPAR) == 0);
        bds_list_print(list, PRINT_NODE);

        // bds_list_insert_sort(list, &key);
        s[0] = create_super_test(43, 42);
        assert(bds_list_insert_sort(list, &s[0], OBJECT_COMPAR) == 0);
        bds_list_print(list, PRINT_NODE);

        s[2] = create_super_test(22, 21);
        assert(bds_list_insert_sort(list, &s[2], OBJECT_COMPAR) == 0);
        bds_list_print(list, PRINT_NODE);

        s[3] = create_super_test(1, 1);
        assert(bds_list_insert_sort(list, &s[3], OBJECT_COMPAR) == 0);
        bds_list_print(list, PRINT_NODE);

        s[4] = create_super_test(1, 1);
        assert(bds_list_insert_sort(list, &s[4], OBJECT_COMPAR) == 0);
        bds_list_print(list, PRINT_NODE);

        assert(bds_list_remove(list, &s[3], OBJECT_COMPAR_ADDR) == 0);
        bds_list_print(list, PRINT_NODE);
        assert(bds_list_remove(list, &s[4], OBJECT_COMPAR_ADDR) == 0);
        bds_list_print(list, PRINT_NODE);

        bds_list_free(&list);

        return 0;
}
