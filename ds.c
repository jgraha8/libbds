#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static size_t initial_stack_len = 8;

static void *xalloc( size_t size_ )
{
	void *v = malloc( size_ );
	assert( v );
	memset( v, 0, size_ );

	return v;
}

static void *xrealloc( void *v_, size_t size_, size_t new_size_ )
{
	v_ = realloc( v_, new_size_ );
	assert( v_ );
	// Zero the new bytes
	if( new_size_ > size_ )
		memset( v_ + size_, 0, new_size_ - size_ );

	return v_;
}

typedef struct ds_stack_t {
	int top;
	size_t elem_size;
	size_t len;
	void *v;
} ds_stack_t;

void ds_set_stacklen( size_t len_ )
{
	initial_stack_len = len_;
}

size_t ds_stack_getlen( ds_stack_t *this_ ) 
{
	return this_->len;
}

int ds_stack_getnelem( ds_stack_t *this_ )
{
	return this_->top+1;
}

void ds_stack_ctor( ds_stack_t *this_, size_t elem_size_ )
{
	memset( this_, 0, sizeof(*this_) );

	this_->top = -1;
	this_->elem_size = elem_size_;
	this_->len = initial_stack_len;
	this_->v = xalloc( this_->len * elem_size_ );
}

void ds_stack_dtor( ds_stack_t *this_ )
{
	free( this_->v );
	memset(this_, 0, sizeof(*this_) );
}

void ds_stack_push( ds_stack_t *this_, const void *data_ )
{
	if( this_->top == this_->len - 1 ) {
		size_t size = this_->len * this_->elem_size;
		
		this_->v = xrealloc( this_->v, size, 2*size );
		this_->len *= 2;
	}
	
	this_->top++;
	memcpy( this_->v + this_->top * this_->elem_size, data_, this_->elem_size );
}

int ds_stack_pop( ds_stack_t *this_, void *data_ )
{
	if( this_->top == -1 )
		return -1;
	
	memcpy( data_, this_->v + this_->top*this_->elem_size, this_->elem_size );
	this_->top--;

	return 0;
}


typedef struct listnode {
	size_t elem_size;
	void *v;
	struct listnode *prev;
} listnode_t;


listnode_t *listnode_alloc( size_t elem_size_, const void *v_ )
{
	listnode_t *node = xalloc( sizeof(*node) );
	
	node->elem_size = elem_size_;
	node->v = xalloc( elem_size_ );
	memcpy( node->v, v_, elem_size_ );

	return node;
}

void listnode_free( listnode_t **this_ )
{
	listnode_t *node = *this_;

	if( node == NULL ) return;
	free( node->v );
	free( node );

	*this_ = NULL;
}

void listnode_freeall( listnode_t **this_ )
{
	listnode_t *node;

	while( *this_ ) {
		node = *this_;
		*this_ = node->prev;
		listnode_free( &node );
	}
}

listnode_t *listnode_push( listnode_t *this_, size_t elem_size_, const void *v_ )
{
	listnode_t *node = listnode_alloc( elem_size_, v_ );

	node->prev = this_;
	return node;
}

listnode_t *listnode_pop( listnode_t *this_, void *v_ )
{
	if( this_ == NULL ) return NULL;

	listnode_t *node = this_;
	this_ = this_->prev;

	memcpy( v_, node->v, node->elem_size );
	listnode_free( &node );

	return this_;
}

typedef struct ds_liststack {
	listnode_t *top;
	size_t elem_size;
	size_t n_elem;
} ds_liststack_t;


void ds_liststack_ctor( ds_liststack_t *this_, size_t elem_size_ )
{
	memset(this_,0,sizeof(*this_) );

	this_->elem_size = elem_size_;
}

void ds_liststack_dtor( ds_liststack_t *this_ )
{
	listnode_freeall( &this_->top );
	memset( this_, 0, sizeof(*this_) );
}

void ds_liststack_push( ds_liststack_t *this_, const void *v_ )
{
	this_->top = listnode_push( this_->top, this_->elem_size, v_ );
	this_->n_elem++;
}

int ds_liststack_pop( ds_liststack_t *this_, void *v_ )
{
	if( this_->top == NULL ) return -1;
	
	this_->top = listnode_pop( this_->top, v_ );
	this_->n_elem--;
	
	return 0;
}

int ds_liststack_getnelem( const ds_liststack_t *this_ )
{
	return this_->n_elem;
}


typedef struct rbnode {
	unsigned char color;
	void *key;
	struct rbnode *left, *right;
	struct rbnode *parent;
} rbnode_t;

rbnode *rbnode_alloc( size_t key_size_, const void *key_ )
{
	rbnode_t *node = xalloc( sizeof(rbnode_t) );
	
	node->key = malloc( key_size_ );
	memcpy( node->key, key_, key_size_ );
}

typedef struct {
	size_t key_size;
	int (*key_compare)( const void *, const void * );
	rbnode_t *root;
} ds_rbtree_t;

void ds_rbtree_ctor( ds_rbtree_t *this_, size_t key_size_, int (*key_compare)( const void *, const void *) )
{
	memset( this_, 0, sizeof(*this_) );
	this_->key_size = key_size_;
}

void rbtree_leftrotate( ds_rbtree_t *this_, rbnode_t *x_ )
{
	rbnode_t *y = x_->right;

	// Assign x.right and set parent
	x_->right = y->left;
	if( x_->right ) {
		x->right->parent = x;
	}

	// Set the parent of y and adjust parent
	y->parent = x_->parent;
	if( y->parent == NULL ) {
		this_->root = y; // Reset tree root
	} else if( y->parent->left == x_ ) {
		y->parent->left = y;
	} else {
		y->parent->right = y;
	}

	y->left = x;
	x_->parent = y;
	
}

void rbtree_rightrotate( ds_rbtree_t *this_, rbnode_t *y_ )
{
	rbnode_t *x = y_->left;

	// Assign x.right and set parent
	y_->left = x->right;
	if( y_->left ) {
		y_->left->parent = y;
	}

	// Set the parent of y and adjust parent
	x->parent = y_->parent;
	if( x->parent == NULL ) {
		this_->root = x; // Reset tree root
	} else if( x->parent->left == y_ ) {
		x->parent->left = x;
	} else {
		x->parent->right = x;
	}

	x->right = y;
	y_->parent = x;
}

void rbtree_insert_fixup( ds_rbtree_t *this_, rbnode_t *z_ )
{
	rbnode_t *zp;
	rbnode_t *zpp;
	rbnode_t *y;

	zp = z_->parent;
	
	while( zp->color == RB_RED ) {

		zpp = zp->parent;
		
		if( zp == zpp->left ) {
			y = zpp->right;
			if( y->color == RB_RED ) {
				zp->color = RB_BLACK;
				y->color = RB_BLACK;
				zpp->color = RB_RED;
				z_ = zpp;
			} else if( 
		}


void ds_rbtree_insert( ds_rbtree_t *this_, void *key_ )
{
	rbnode_t *x = &this_->root;
	rbnode_t *y = NULL;
	rbnode_t *z = rbnode_alloc( this_->key_size, key_ );

	while ( x ) {
		y = x;
		if( this_->key_compare( z->key, x->key ) < 0 ) {
			x = x->left;
		} else {
			x = x->right;
		}
	}
	z->parent = y;

	if( y == NULL ) {
		this_->root = z;
	} else if( this_->key_compare( z->key, y->key ) < 0 ) {
		y->left = z;
	} else {
		y->right = z;
	}
	z->color = RB_RED;

	rbtree_insert_fixup( this_, z );
}


int main(void)
{
	ds_stack_t stack;

	int i,j;

	ds_set_stacklen( 1 );
	
	ds_stack_ctor( &stack, sizeof(int) );

#define N 10
	
	for( i=0; i<N; i++ ) {
		ds_stack_push( &stack, &i );
	}

	printf("nelem = %d\n", ds_stack_getnelem( &stack ) );

	for( i=0; i<N+1; i++ ) {
		if( ds_stack_pop( &stack, &j ) == 0 ) {
			printf("j = %d\n",j);
		} else {
			printf("empty stack\n");
		}
	}
	for( i=0; i<N; i++ ) {
		ds_stack_push( &stack, &i );
	}

	for( i=0; i<N+1; i++ ) {
		if( ds_stack_pop( &stack, &j ) == 0 ) {
			printf("j = %d\n",j);
		} else {
			printf("empty stack\n");
		}
	}

	ds_liststack_t lstack;
	ds_liststack_ctor( &lstack, sizeof(int) );
	
	for( i=0; i<N; i++ ) {
		ds_liststack_push( &lstack, &i );
	}

	printf("nelem = %d\n", ds_liststack_getnelem( &lstack ) );	
	for( i=0; i<N+1; i++ ) {
		if( ds_liststack_pop( &lstack, &j ) == 0 ) {
			printf("j = %d\n",j);
		} else {
			printf("empty stack\n");
		}
	}
	printf("nelem = %d\n", ds_liststack_getnelem( &lstack ) );			
	
	ds_stack_dtor( &stack );
	ds_liststack_dtor( &lstack );

	return 0;
}
