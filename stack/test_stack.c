#include <libbds/bds_stack.h>

#define N 128

static void noop_dtor(void *v) { return; }

#define TEST(TYPE)                                                                                                     \
        static int test_##TYPE(struct bds_stack *stack)                                                                \
        {                                                                                                              \
                bds_stack_dtor(stack);                                                                                 \
                bds_stack_ctor(stack, 1, sizeof(TYPE), noop_dtor);                                                     \
                                                                                                                       \
                if (!bds_stack_isempty(stack))                                                                         \
                        return 1;                                                                                      \
                                                                                                                       \
                int i;                                                                                                 \
                TYPE a, b;                                                                                             \
                const TYPE *c;                                                                                         \
                for (i = 0; i < N; ++i) {                                                                              \
                        a = (TYPE)i;                                                                                   \
                        bds_stack_push(stack, &a);                                                                     \
                }                                                                                                      \
                                                                                                                       \
                for (i = N - 1; i >= 1; --i) {                                                                         \
                        if ((c = bds_stack_pop(stack, &b)) == NULL)                                                    \
                                return -1;                                                                             \
                                                                                                                       \
                        a = (TYPE)i;                                                                                   \
                        if (b != a || *c != a)                                                                         \
                                return 1;                                                                              \
                }                                                                                                      \
                return 0;                                                                                              \
        }

#define test(TYPE) test_##TYPE

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ulonglong;
typedef long long longlong;

TEST(uchar);
TEST(ushort);
TEST(uint);
TEST(ulong);
TEST(ulonglong);
TEST(char);
TEST(short);
TEST(int);
TEST(long);
TEST(longlong);
TEST(float);
TEST(double);

int main()
{
        struct bds_stack stack;

        bds_stack_ctor(&stack, 1, 1, noop_dtor);

        if (test(char)(&stack) != 0)
                return 1;
        if (test(short)(&stack) != 0)
                return 2;
        if (test(int)(&stack) != 0)
                return 3;
        if (test(long)(&stack) != 0)
                return 4;
        if (test(longlong)(&stack) != 0)
                return 5;
        if (test(uchar)(&stack) != 0)
                return 6;
        if (test(ushort)(&stack) != 0)
                return 7;
        if (test(uint)(&stack) != 0)
                return 8;
        if (test(ulong)(&stack) != 0)
                return 9;
        if (test(ulonglong)(&stack) != 0)
                return 10;
        if (test(float)(&stack) != 0)
                return 11;
        if (test(double)(&stack) != 0)
                return 12;

        bds_stack_dtor(&stack);

        return 0;
}
