#include <assert.h>
#include <stdio.h>
#include <libbds/bds_queue.h>

#define N 8

static void print_queue( struct bds_queue *queue )
{
	printf("========================================\n");
	printf("n_alloc   = %zd\n", queue->n_alloc   );
	printf("n_elem    = %zd\n", queue->n_elem    );
	printf("elem_len  = %zd\n", queue->elem_len );
	printf("front     = %zd\n", queue->front);
	printf("back      = %zd\n", bds_queue_back( queue ) );
	printf("v = ");

	int i;
	const int *v = queue->v;
	for( i=0; i<queue->n_alloc; ++i ) {
		printf("%d ", *v++);
	}
	printf("\n");
}

int main()
{
	struct bds_queue *queue = bds_queue_alloc( 3, sizeof(int), NULL );
	int i;

	if( !bds_queue_get_autoresize(queue) )
		bds_queue_set_autoresize(queue, true);
	
	print_queue( queue );
	for( i=0; i<N; ++i ) {
		assert( bds_queue_push( queue, &i ) == 0 );
		print_queue( queue );
		printf("cmd: push\n");
	}

	for ( i=0; i<N-1; ++i ) {
		bds_queue_pop( queue, NULL );
		printf("cmd: pop\n");		
	}
	print_queue( queue );
	
	for( i=N; i<(N+N+N); ++i ) {
		assert( bds_queue_push( queue, &i ) == 0 );
		print_queue(queue);
		printf("cmd: push\n");		
	}
	print_queue(queue);

	bds_queue_free( &queue );
}
