#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libbds/bds_list.h>

int compar_int(const void *a, const void *b) { return (*(int *)a - *(int *)b); }

#define OBJECT_COMPAR compar_int
#define OBJECT_DTOR NULL

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*a))

//#define print_list(a, b)

int main(void)
{
        struct bds_list *list = bds_list_alloc(sizeof(int), OBJECT_DTOR);

        int i[] = {7, 6, 5, 4, 3, 2, 1, 0};

        for( int n=0; n<ARRAY_SIZE(i); ++n )
		assert(bds_list_insert_sort(list, &i[n], OBJECT_COMPAR) == 0);

	struct bds_list_node *node = bds_list_begin(list);
	
        for( int n=0; node != bds_list_end(); ++n, node = bds_list_iterate(node) )
		assert( i[ARRAY_SIZE(i)-1-n] == *(int *)bds_list_object(node) );

        bds_list_free(&list);

        return 0;
}
