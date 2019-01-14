/*
 * Copyright (C) 2017 Jason Graham <jgraham@compukix.net>
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
