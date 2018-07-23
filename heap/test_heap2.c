#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <libbds/bds_heap.h>

#include "heap/heap_util.h"

#define N 20

int compar(const void *a, const void *b)
{
        return (*((const int *)a) - *((const int *)b));
}

int main(int argc, char **argv)
{
        int a[N];
        int a_min[N];
        int a_save[2][N];

        srand(time(NULL));
        for (int i = 0; i < N; ++i) {
                a[i] = rand() % 100;
        }

        // struct bds_heap *heap = bds_heap_alloc(BDS_HEAP_MAX, N, sizeof(a[0]), compar, a);
        struct bds_heap *heap = bds_heap_alloc(BDS_HEAP_MIN, 1, sizeof(a[0]), compar, NULL);

        bds_heap_assign(heap, a, N);

        assert(bds_heap_size(heap) == N);

        bds_heap_build(heap);
        memcpy(&a[0], bds_heap_ptr(heap), N * sizeof(a[0]));

        for (int i = 1; i < N; ++i) {
                assert(a[PARENT(i)] <= a[i]);
        }

        const int key = 101;
        bds_heap_change_key(heap, N / 2, &key);

        memcpy(a_save[0], bds_heap_ptr(heap), N * sizeof(a[0]));
        memcpy(a_save[1], bds_heap_ptr(heap), N * sizeof(a[0]));

        for (int val = 0, i = 0; bds_heap_pop(heap, &val) == 0; a_min[i++] = val);

        bds_heap_free(&heap);

        bds_heapsort(a_save[0], N, sizeof(a[0]), compar);
        qsort(a_save[1], N, sizeof(a[0]), compar);

        assert(memcmp(a_min, a_save[0], N * sizeof(a[0])) == 0);
        assert(memcmp(a_min, a_save[1], N * sizeof(a[0])) == 0);

        return 0;
}
