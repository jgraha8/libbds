#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PARENT(i) (((i + 1) >> 1) - 1)
#define LEFT(i) (((i + 1) << 1) - 1)
#define RIGHT(i) ((i + 1) << 1)
#define PTR(heap, i) (heap->data + heap->elem_len * (i))
#define MAX(a, b) (a > b ? a : b)

#define BDS_HEAP_MAX (1 << 0)
#define BDS_HEAP_MIN (1 << 1)

static inline void memswap(void *a, void *b, size_t elem_len)
{
        char tmp[elem_len];
        memcpy(tmp, a, elem_len);
        memcpy(a, b, elem_len);
        memcpy(b, tmp, elem_len);
}

struct bds_heap {
        void *data;
        int (*compar)(const void *, const void *);
        void (*__heapify)(struct bds_heap *, int);
        void (*__reparent)(struct bds_heap *, int);
        void (*__build)(struct bds_heap *);
        void (*__change_key)(struct bds_heap *, int, const void *);

        size_t elem_len;
        int num_elem;
        int num_alloc;
        int heap_type;
        bool data_owner;
};

static void max_heapify(struct bds_heap *heap, int node)
{
        int left  = LEFT(node);
        int right = RIGHT(node);

        if (left >= heap->num_elem && right >= heap->num_elem)
                return;

        int c       = -INT_MAX;
        int largest = -1;

        if (left < heap->num_elem) {
                int __c = heap->compar(PTR(heap, left), PTR(heap, node));
                if (__c > 0 && __c > c) {
                        c       = __c;
                        largest = left;
                }
        }
        if (right < heap->num_elem) {
                int __c = heap->compar(PTR(heap, right), PTR(heap, node));
                if (__c > 0 && __c > c) {
                        c       = __c;
                        largest = right;
                }
        }

        if (largest > 0) {
                memswap(PTR(heap, largest), PTR(heap, node), heap->elem_len);
                max_heapify(heap, largest);
        }
}

static void max_build(struct bds_heap *heap)
{
        for (int n = heap->num_elem / 2 - 1; n >= 0; --n) {
                heap->__heapify(heap, n);
        }
}

// Flow upwards
static void max_reparent(struct bds_heap *heap, int node)
{
        if (node <= 0)
                return;

        int parent = PARENT(node);

        if (heap->compar(PTR(heap, parent), PTR(heap, node)) < 0) {
                memswap(PTR(heap, parent), PTR(heap, node), heap->elem_len);
                max_reparent(heap, parent);
        }
}

static void max_change_key(struct bds_heap *heap, int node, const void *val)
{
        int c = heap->compar(val, PTR(heap, node));

        if (c == 0)
                return;

        memcpy(PTR(heap, node), val, heap->elem_len);

        if (c < 0) {
                heap->__heapify(heap, node);
        } else {
                heap->__reparent(heap, node);
        }
}

struct bds_heap *bds_heap_alloc(int heap_type, int num_alloc, size_t elem_len,
                                int (*compar)(const void *, const void *), void *data)
{
        struct bds_heap *heap = calloc(1, sizeof(*heap));
        assert(heap);

        heap->num_alloc = num_alloc;
        heap->elem_len  = elem_len;
        heap->compar    = compar;

        if (data) {
                heap->num_elem = num_alloc;
                heap->data     = data;
        } else {
                heap->data       = calloc(num_alloc, elem_len);
                heap->data_owner = true;
                assert(heap->data);
        }

        heap->heap_type = heap_type;

        if (heap_type == BDS_HEAP_MAX) {
                heap->__heapify    = max_heapify;
                heap->__reparent   = max_reparent;
                heap->__build      = max_build;
                heap->__change_key = max_change_key;
        } else {
                fprintf(stderr,"incorrect heap type\n");
                exit(EXIT_FAILURE);
        }

        return heap;
}

void bds_heap_free(struct bds_heap **heap)
{
        if (*heap == NULL)
                return;

        if ((*heap)->data_owner) {
                free((*heap)->data);
        }
        free(*heap);
        *heap = NULL;
}

void bds_heap_heapify(struct bds_heap *heap, int node) { heap->__heapify(heap, node); }
void bds_heap_reparent(struct bds_heap *heap, int node) { heap->__reparent(heap, node); }
void bds_heap_build(struct bds_heap *heap) { heap->__build(heap); }
void bds_heap_change_key(struct bds_heap *heap, int node, const void *val) { heap->__change_key(heap, node, val); }

int bds_heap_pop(struct bds_heap *heap, void *val)
{
        if (heap->num_elem == 0)
                return 1;

        memcpy(val, PTR(heap, 0), heap->elem_len);
        memcpy(PTR(heap, 0), PTR(heap, heap->num_elem - 1), heap->elem_len);

        --heap->num_elem;
        heap->__heapify(heap, 0);

        return 0;
}
int bds_heap_insert(struct bds_heap *heap, const void *val)
{
        if (heap->num_elem == heap->num_alloc)
                return 1;

        ++heap->num_elem;
        memcpy(PTR(heap, heap->num_elem - 1), val, heap->elem_len);
        heap->__reparent(heap, heap->num_elem - 1);

        return 0;
}

int bds_heap_get(struct bds_heap *heap, void *val)
{
        if (heap->num_elem == 0)
                return 1;

        memcpy(val, PTR(heap, 0), heap->elem_len);
        return 0;
}

static void heapsort(void *data, int num_elem, size_t elem_len, int (*compar)(const void *, const void *))
{
        struct bds_heap *heap = bds_heap_alloc(BDS_HEAP_MAX, num_elem, elem_len, compar, data);

        bds_heap_build(heap);

        while (heap->num_elem > 1) {
                memswap(PTR(heap, 0), PTR(heap, heap->num_elem - 1), heap->elem_len);
                --heap->num_elem;
                bds_heap_heapify(heap, 0);
        }

        bds_heap_free(&heap);
}

#define N 20

int compar(const void *a, const void *b) { return (*((const int *)a) - *((const int *)b)); }

int main(int argc, char **argv)
{
        int a[N];
        int a_save[N];

        srand(time(NULL));
        for (int i = 0; i < N; ++i) {
                a[i] = rand() % 100;
        }

	//struct bds_heap *heap = bds_heap_alloc(BDS_HEAP_MAX, N, sizeof(a[0]), compar, a);
        struct bds_heap *heap = bds_heap_alloc(BDS_HEAP_MAX, N, sizeof(a[0]), compar, NULL);
	memcpy(heap->data, &a[0], N*sizeof(a[0]));
	heap->num_elem = N;

        bds_heap_build(heap);
	
        for (int i = 1; i < N; ++i) {
                assert(a[PARENT(i)] >= a[i]);
        }

	const int key = 101;
	bds_heap_change_key(heap,N-1, &key); 

        int val;
        printf("a = { ");
        for (int i = 1; i < N; ++i) {
                // assert(a[PARENT(i)] >= a[i]);
                assert(bds_heap_pop(heap, &val) == 0);
                printf("%d ", val);
        }
        printf("}\n");

        bds_heap_free(&heap);

        printf("a = { ");
        for (int i = 0; i < N; ++i) {
                a[i]      = rand() % 100;
                a_save[i] = a[i];
                printf("%d ", a[i]);
        }
        printf("}\n");

        heapsort(a, N, sizeof(a[0]), compar);
        qsort(a_save, N, sizeof(a[0]), compar);

        if (memcmp(a, a_save, N * sizeof(a[0])) != 0) {
                fprintf(stderr, "failed\n");
                // exit(EXIT_FAILURE);
        }

        printf("a = { ");
        for (int i = 0; i < N; ++i) {
                printf("%d ", a[i]);
        }
        printf("}\n");

        return 0;
}
