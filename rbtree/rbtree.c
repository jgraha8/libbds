/*
 * Red-Black Tree Data Structure Module
 *
 * Copyright (C) 2017,2020 Jason Graham <jgraham@compukix.net>
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

#include "memutil.h"
#include <libbds/bds_rbtree.h>

#define RB_BLACK (0U)
#define RB_RED   (1U)

typedef struct bds_rbnode {
	void *key;
	struct bds_rbnode *parent;
	struct bds_rbnode *left, *right;
	unsigned char color;
} bds_rbnode_t;

void *bds_rbnode_key(bds_rbnode_t *rbnode)
{
	return rbnode->key;
}

#define NIL (&NIL_NODE)

static bds_rbnode_t NIL_NODE = { .key = NULL,
				 .parent = NIL,
				 .left   = NIL,
				 .right  = NIL,
				 .color  = RB_BLACK };

static bds_rbnode_t *rbnode_alloc( size_t key_len, const void *key )
{
	bds_rbnode_t *node = xalloc( ALIGN16_LEN(sizeof(bds_rbnode_t)) + key_len );

	*node = *NIL;
	
	node->key = (char *)node + ALIGN16_LEN(sizeof(bds_rbnode_t));
	memcpy( node->key, key, key_len );

	return node;
}

static void rbnode_free(bds_rbnode_t **node, void (*key_dtor)(void *))
{
	if( *node == NULL ) {
		return;
	}

	if( (*node)->key && key_dtor ) {
		key_dtor((*node)->key);
	}

	free(*node);
	*node = NULL;
}

static bds_rbnode_t *tree_minimum(bds_rbnode_t *x)
{
	while( x->left != NIL ) {
		x = x->left;
	}
	return x;
}

static bds_rbnode_t *tree_maximum(bds_rbnode_t *x)
{
	while( x->right != NIL ) {
		x = x->right;
	}
	return x;
}

static void tree_delete(bds_rbnode_t *x, void (*key_dtor)(void *))
{
	if( x->left != NIL ) {
		tree_delete(x->left, key_dtor);
	}
	if( x->right != NIL ) {
		tree_delete(x->right, key_dtor);
	}

	rbnode_free(&x, key_dtor);
}

typedef struct bds_rbtree {
	size_t key_len;
	int (*key_compare)( const void *, const void * );
	void (*key_dtor)(void *);
	size_t num_elem;	
	bds_rbnode_t *root;
} bds_rbtree_t;

static void rbtree_left_rotate(bds_rbtree_t *rb, bds_rbnode_t *x);

static void rbtree_right_rotate(bds_rbtree_t *rb, bds_rbnode_t *x);

static void rbtree_insert_fixup(bds_rbtree_t *rb, bds_rbnode_t *z);

static void rbtree_transplant(bds_rbtree_t *rb, bds_rbnode_t *u, bds_rbnode_t *v);

static void rbtree_delete_fixup(bds_rbtree_t *rb, bds_rbnode_t *x);

static bds_rbnode_t *rbtree_search(bds_rbtree_t *rb, const void *key);

bds_rbtree_t *bds_rbtree_alloc(size_t key_len,
			       int (*key_compare)( const void *, const void * ),
			       void (*key_dtor)(void *))
{
	bds_rbtree_t *rb = xalloc(sizeof(*rb));

	rb->key_len = key_len;
	rb->key_compare = key_compare;
	rb->key_dtor = key_dtor;
	rb->root = NIL;

	return rb;
}

void bds_rbtree_free(bds_rbtree_t **rb)
{
	if( *rb == NULL ) {
		return;
	}

	bds_rbtree_clear(*rb);
	free(*rb);
	*rb = NULL;
}

size_t bds_rbtree_size(const bds_rbtree_t *rb)
{
	return rb->num_elem;
}

void bds_rbtree_clear(bds_rbtree_t *rb)
{
	tree_delete(rb->root, rb->key_dtor);
	rb->num_elem = 0;
	
}

void *bds_rbtree_search(bds_rbtree_t *rb, const void *key)
{
	bds_rbnode_t *x = rbtree_search(rb, key);

	if( x == NIL ) {
		return NULL;
	}

	return x->key;
}

void bds_rbtree_insert(bds_rbtree_t *rb, const void *key)
{
	bds_rbnode_t *y = NIL;
	bds_rbnode_t *x = rb->root;
	bds_rbnode_t *z = rbnode_alloc(rb->key_len, key);

	while( x != NIL ) {
		y = x;
		if( rb->key_compare(z->key, x->key) < 0 ) {
			x = x->left;
		} else {
			x = x->right;
		}
	}

	z->parent = y;
	if( y == NIL ) {
		rb->root = z;
	} else if( rb->key_compare(z->key, y->key) < 0 ) {
		y->left = z;
	} else {
		y->right = z;
	}

	z->color = RB_RED;

	rbtree_insert_fixup(rb, z);
	++rb->num_elem;
}

void bds_rbtree_delete(bds_rbtree_t *rb, const void *key)
{
	bds_rbnode_t *z = rbtree_search(rb, key);
	if( z == NIL ) {
		return;
	}
	
	bds_rbnode_t *x = NULL;
	bds_rbnode_t *y = z;
	unsigned char y_original_color = y->color;

	if( z->left == NIL ) {
		x = z->right;
		rbtree_transplant(rb, z, z->right);
	} else if( z->right == NIL ) {
		x = z->left;
		rbtree_transplant(rb, z, z->left);
	} else {
		y = tree_minimum(z->right);
		y_original_color = y->color;

		x = y->right;
		if( y->parent == z ) {
			x->parent = y;
		} else {
			rbtree_transplant(rb, y, y->right);
			y->right = z->right;
			y->right->parent = y;
		}

		rbtree_transplant(rb, z, y);
		y->left = z->left;
		y->left->parent = y;
		y->color = z->color;
	}

	if( y_original_color == RB_BLACK ) {
		rbtree_delete_fixup(rb, x);
	}

	rbnode_free(&z, rb->key_dtor);
	assert( rb->num_elem-- > 0 );
}



bds_rbnode_t *bds_rbtree_iterate_begin(bds_rbtree_t *rb)
{
	return tree_minimum(rb->root);
}

const bds_rbnode_t *bds_rbtree_iterate_end()
{
	return (const bds_rbnode_t *)NIL;
}

/*
//
//             x
//            / \
//           /   \
//          x     x
//         / \   / \
//        x   x x   x
//           / \
//          x   x
//
//
*/
bds_rbnode_t *bds_rbtree_iterate_next(bds_rbtree_t *rb, bds_rbnode_t *iter_node)
{
	if( iter_node == NIL ) {
		return NIL;
	}

	bds_rbnode_t *x = iter_node;

	if( x->right != NIL ) {
		return tree_minimum(x->right);
	}

	if( x == x->parent->left ) {
		return x->parent;
	}

	while( x->parent != NIL && x->parent->right == x ) {
		x = x->parent;
	}

	return x->parent;
}

static void rbtree_left_rotate(bds_rbtree_t *rb, bds_rbnode_t *x)
{
	bds_rbnode_t *y = x->right;

	x->right = y->left;

	if (y->left != NIL ) {
		y->left->parent = x;
	}

	y->parent = x->parent;
	if( x->parent == NIL ) {
		rb->root = y;
	} else if( x == x->parent->left ) {
		x->parent->left = y;
	} else {
		x->parent->right = y;
	}

	y->left = x;
	x->parent = y;
}

static void rbtree_right_rotate(bds_rbtree_t *rb, bds_rbnode_t *x)
{
	bds_rbnode_t *y = x->left;

	x->left = y->right;

	if (y->right != NIL ) {
		y->right->parent = x;
	}

	y->parent = x->parent;
	if( x->parent == NIL ) {
		rb->root = y;
	} else if( x == x->parent->left ) {
		x->parent->left = y;
	} else {
		x->parent->right = y;
	}

	y->right = x;
	x->parent = y;
}

static void rbtree_insert_fixup(bds_rbtree_t *rb, bds_rbnode_t *z)
{
	while( z->parent->color == RB_RED ) {
		if( z->parent == z->parent->parent->left ) {
			bds_rbnode_t *y = z->parent->parent->right;
			if( y->color == RB_RED ) {            // Case 1
				z->parent->color = RB_BLACK;
				y->color = RB_BLACK;
				z->parent->parent->color = RB_RED;

				z = z->parent->parent;
			} else {
				if( z == z->parent->right ) { // Case 2
					z = z->parent;
					rbtree_left_rotate(rb, z);
				}

				z->parent->color = RB_BLACK;   // Case 3
				z->parent->parent->color = RB_RED;
				rbtree_right_rotate(rb, z->parent->parent);
			}
		} else {
			bds_rbnode_t *y = z->parent->parent->left;
			if( y->color == RB_RED ) {            // Case 1
				z->parent->color = RB_BLACK;
				y->color = RB_BLACK;
				z->parent->parent->color = RB_RED;

				z = z->parent->parent;
			} else {
				if( z == z->parent->left ) { // Case 2
					z = z->parent;
					rbtree_right_rotate(rb, z);
				}

				z->parent->color = RB_BLACK;   // Case 3
				z->parent->parent->color = RB_RED;
				rbtree_left_rotate(rb, z->parent->parent);
			}
		}
	}
	
	rb->root->color = RB_BLACK;
}

static void rbtree_transplant(bds_rbtree_t *rb, bds_rbnode_t *u, bds_rbnode_t *v)
{
	if( u->parent == NIL) {
		rb->root = v;
	} else if( u == u->parent->left ) {
		u->parent->left = v;
	} else {
		u->parent->right = v;
	}

	v->parent = u->parent;
}

static void rbtree_delete_fixup(bds_rbtree_t *rb, bds_rbnode_t *x)
{
	while( x != NIL && x->color == RB_BLACK ) {
		if( x == x->parent->left ) {
			bds_rbnode_t *w = x->parent->right;
			if( w->color == RB_RED ) {                 // Case 1
				w->color = RB_BLACK;
				x->parent->color = RB_RED;
				rbtree_left_rotate(rb, x->parent);
				w = x->parent->right;
			}
			if( w->left->color == RB_BLACK && w->right->color == RB_BLACK ) { // Case 2
				w->color = RB_RED;
				x = x->parent;
			} else {
				if( w->right->color == RB_BLACK ) {  // Case 3
					w->left->color = RB_BLACK;
					w->color = RB_RED;
					rbtree_right_rotate(rb, w);
					w = x->parent->right;
				}

				w->color = x->parent->color;          // Case 4
				x->parent->color = RB_BLACK;
				w->right->color = RB_BLACK;
				rbtree_left_rotate(rb, x->parent);
				x = rb->root;
			}
		} else {
			bds_rbnode_t *w = x->parent->left;
			if( w->color == RB_RED ) {                 // Case 1
				w->color = RB_BLACK;
				x->parent->color = RB_RED;
				rbtree_right_rotate(rb, x->parent);
				w = x->parent->left;
			}
			if( w->right->color == RB_BLACK && w->left->color == RB_BLACK ) { // Case 2
				w->color = RB_RED;
				x = x->parent;
			} else {
				if( w->left->color == RB_BLACK ) {  // Case 3
					w->right->color = RB_BLACK;
					w->color = RB_RED;
					rbtree_left_rotate(rb, w);
					w = x->parent->left;
				}

				w->color = x->parent->color;          // Case 4
				x->parent->color = RB_BLACK;
				w->left->color = RB_BLACK;
				rbtree_right_rotate(rb, x->parent);
				x = rb->root;
			}
		}
	}

	x->color = RB_BLACK;
}

static bds_rbnode_t *rbtree_search(bds_rbtree_t *rb, const void *key)
{
	bds_rbnode_t *x = rb->root;
	int k;
	
	while( x != NIL && (k = rb->key_compare(key, x->key)) != 0 ) {
		if( k < 0 ) {
			x = x->left;
		} else {
			x = x->right;
		}
	}

	return x;
}
