#include <libbds/bds_stack.h>

#define N 128

static int int_test( struct bds_stack *stack )
{
	bds_stack_dtor( stack );
	bds_stack_ctor( stack, 1, sizeof(int));

	if( ! bds_stack_isempty( stack ) ) return 1;

	int i,j;
	const int *k;
	for( i=0; i<N; ++i ) {
		bds_stack_push( stack, &i );
	}

	for( i=N-1; i>=0; --i ) {
		if( ( k = bds_stack_pop( stack, &j ) ) == NULL )
			return -1;
		if( j != i || *k != i ) return 1;
	}
	return 0;
}

static int double_test( struct bds_stack *stack )
{
	bds_stack_dtor( stack );
	bds_stack_ctor( stack, 1, sizeof(double));

	if( ! bds_stack_isempty( stack ) ) return 1;

	int i;
	double a,b;
	const double *c;
	for( i=0; i<N; ++i ) {
		a = (double)i; 
		bds_stack_push( stack, &a );
	}

	for( i=N-1; i>=0; --i ) {
		if( ( c = bds_stack_pop( stack, &b ) ) == NULL ) 
			return -1;

		a = (double)i;
		if( b != a || *c != a ) return 1;
	}

	return 0;
}


int main()
{
	struct bds_stack stack;
	
	bds_stack_ctor( &stack, 1, 1 );

	if( int_test( &stack ) != 0 ) return 1;
	if( double_test( &stack ) != 0 ) return 2;

	bds_stack_dtor( &stack );
	
	return 0;
}
