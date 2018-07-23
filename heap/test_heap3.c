#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BDS_NAMESPACE
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
	const int *a_ptr;

        srand(time(NULL));
        for (int i = 0; i < N; ++i) {
                a[i] = rand() % 100;
        }

        // struct bds_heap *heap = bds_heap_alloc(BDS_HEAP_MAX, N, sizeof(a[0]), compar, a);
        heap_t *heap = heap_alloc(HEAP_MIN, 1, sizeof(a[0]), compar, NULL);

	for( int i=0; i<N; ++i ) {
		heap_insert(heap, &a[i]);
	}
        assert(heap_size(heap) == N);

        heap_build(heap);

	a_ptr = heap_ptr(heap);

        for (int i = 1; i < N; ++i) {
                assert(a_ptr[PARENT(i)] <= a_ptr[i]);
        }

        return 0;
}
