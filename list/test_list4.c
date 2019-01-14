/*
 * Copyright (C) 2017-2018 Jason Graham <jgraham@compukix.net>
 *
 * This file is part of libbds.
 *
 * libbds is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * libbds is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libbds. If not, see
 * <https://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libbds/bds_list.h>

void print_int(const void *object) { printf("%d", *(int *)object); }

int compar_int(const void *a, const void *b) { return (*(int *)a - *(int *)b); }

#define OBJECT_COMPAR compar_int
#define OBJECT_DTOR NULL

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*a))

//#define print_list(a, b)

int main(void)
{
        struct bds_list *list = bds_list_alloc(sizeof(int), OBJECT_DTOR);

        int i[] = {0, 1, 2, 3, 4, 5, 6, 7 };

	bds_list_append(list, &i[0] );
	
        for( int n=1; n<ARRAY_SIZE(i); ++n )
		assert(bds_list_insert_before(list, &i[n], &i[0], OBJECT_COMPAR) == 0);

	bds_list_print(list, print_int);

	struct bds_list_node *node = bds_list_begin(list);
	
        for( int n=0; n < ARRAY_SIZE(i)-1; ++n, node = bds_list_iterate(node) )
		assert( i[n+1] == *(int *)bds_list_object(node) );

	assert( i[3] == *(int *)bds_list_lsearch(list, &i[3], OBJECT_COMPAR) );

        bds_list_free(&list);

        return 0;
}
