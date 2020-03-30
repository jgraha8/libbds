/*
 * Red-Black Tree Data Structure Test 1 Program
 *
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
#include <time.h>
#include <stdlib.h>

#include <libbds/bds_rbtree.h>

#define N 1000000

struct my_struct {
	int key;
	double value;
};

int compare_my_struct(const void *a, const void *b)
{
	return ((const struct my_struct *)a)->key - ((const struct my_struct *)b)->key;
}

void run_tests()
{	
	bds_rbtree_t *rb = bds_rbtree_alloc(sizeof(struct my_struct), compare_my_struct, NULL);
	struct my_struct *mp;
	int key_prev;
	int key;
	size_t n=0;
	int key_delete = 0;
	int key_search = 0;

	srand(time(NULL));
	
	for( int i=0; i<N; ++i ) {
		int r = 0;
		while(1) {
			r = rand() % (1U << 30);
			struct my_struct m = { .key = r, .value = 2*(double)i };

			if( NULL == bds_rbtree_search(rb, &m) ) {
				bds_rbtree_insert(rb, &m);
				break;
			}
		}

		if( i == N/2 ) {
			key_delete = r;
		}
		if( i == N/4 ) {
			key_search = r;
		}
	}
	assert( N == bds_rbtree_size(rb) );

	n=0;
	for( bds_rbnode_t *node = bds_rbtree_iterate_begin(rb);
	     node != bds_rbtree_iterate_end();
	     node = bds_rbtree_iterate_next(rb, node), ++n ) {

		struct my_struct *mp = bds_rbnode_key(node);
		
		if( n > 0 ) {
			assert(mp->key >= key_prev );
		}
		key_prev = mp->key;
	}
	
	bds_rbtree_delete(rb, &key_delete);
	assert( NULL == bds_rbtree_search(rb, &key_delete));
	assert( N-1 == bds_rbtree_size(rb) );

	assert( (mp = bds_rbtree_search(rb, &key_search)) );
	assert( mp->key == key_search );
	
	n = 0;
	for( bds_rbnode_t *node = bds_rbtree_iterate_begin(rb);
	     node != bds_rbtree_iterate_end();
	     node = bds_rbtree_iterate_next(rb, node), ++n ) {

		mp = bds_rbnode_key(node);
		assert( mp->key != key_delete );
		if( n > 0 ) {
			assert(mp->key >= key_prev );
		}
		key_prev = mp->key;
	}
	assert( n == N-1);

	bds_rbtree_free(&rb);
}
		

int main(int argc, char **argv)
{
	run_tests();
	return 0;
}
