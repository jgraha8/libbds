#ifndef __HEAP_UTIL_H__
#define __HEAP_UTIL_H__

#define PARENT(i) (((i + 1) >> 1) - 1)
#define LEFT(i) (((i + 1) << 1) - 1)
#define RIGHT(i) ((i + 1) << 1)
#define PTR(heap, i) (heap->data + heap->elem_len * (i))

#endif // __HEAP_UTIL_H__
