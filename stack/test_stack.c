#include <ckunkwurx/cku_stack.h>

#define N 128

static int int_test( struct cku_stack *stack )
{
	cku_stack_dtor( stack );
	cku_stack_ctor( stack, 1, sizeof(int));

	if( ! cku_stack_isempty( stack ) ) return 1;

	int i,j;
	int rc;
	for( i=0; i<N; ++i ) {
		cku_stack_push( stack, &i );
	}

	for( i=N-1; i>=0; --i ) {
		rc = cku_stack_pop( stack, &j );
		if( rc != 0 ) return rc;
		if( j != i ) return 1;
	}
	return 0;
}

static int double_test( struct cku_stack *stack )
{
	cku_stack_dtor( stack );
	cku_stack_ctor( stack, 1, sizeof(double));

	if( ! cku_stack_isempty( stack ) ) return 1;

	int i;
	double a,b;
	int rc;
	for( i=0; i<N; ++i ) {
		a = (double)i; 
		cku_stack_push( stack, &a );
	}

	for( i=N-1; i>=0; --i ) {
		rc = cku_stack_pop( stack, &b );
		if( rc != 0 ) return rc;

		a = (double)i;
		if( a != b ) return 1;
	}

	return 0;
}


int main()
{
	struct cku_stack stack;
	
	cku_stack_ctor( &stack, 1, 1 );

	if( int_test( &stack ) != 0 ) return 1;
	if( double_test( &stack ) != 0 ) return 2;

	cku_stack_dtor( &stack );
	
	return 0;
}
