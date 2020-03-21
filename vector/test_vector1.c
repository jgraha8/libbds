/*
 * Copyright (C) 2020 Jason Graham <jgraham@compukix.net>
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

#include <libbds/bds_version.h>
#include <libbds/bds_vector.h>

struct my_data {
	double f;
	int i;
};

int compar_my_data(const struct my_data *a, const struct my_data *b)
{
	return (a->i - b->i);
}

static void test1()
{
	const int N = 10;
	struct my_data data[N];


	for( int i=0; i<N; ++i ) {
		data[i].f = i;
		data[i].i = N - i;
	}
	
	struct bds_vector *vector = bds_vector_alloc(1, sizeof(struct my_data), NULL);

	for( int i=0; i<N; ++i ) {
		if( i > 0 ) {
			assert( compar_my_data(&data[i-1], &data[i]) > 0 );
		}
		bds_vector_insert_sort(vector, &data[i], (int (*)(const void *, const void *))compar_my_data);
	}

	for( int i=0; i<N; ++i ) {
		if( i > 0 ) {
			assert( compar_my_data(bds_vector_get_const(vector, i-1), bds_vector_get_const(vector, i)) < 0 );
		}
	}

	bds_vector_free(&vector);
}


int main(int argc, char **argv)
{
	test1();
	return 0;
}
