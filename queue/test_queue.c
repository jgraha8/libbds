#include <ckunkwurx/cku_queue.h>

#include <stdio.h>

#define N 8

static void print_queue( struct cku_queue *queue )
{
	printf("========================================\n");
	printf("n_alloc   = %zd\n", queue->n_alloc   );
	printf("n_elem    = %zd\n", queue->n_elem    );
	printf("elem_len  = %zd\n", queue->elem_len );
	printf("front     = %zd\n", queue->front);
	printf("back      = %zd\n", cku_queue_back( queue ) );
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
	struct cku_queue *queue = cku_queue_alloc( 3, sizeof(int) );
	int i;


	print_queue( queue );
	for( i=0; i<N; ++i ) {
		cku_queue_push( queue, &i );
		print_queue( queue );
		printf("cmd: push\n");
	}

	for ( i=0; i<N-1; ++i ) {
		cku_queue_pop( queue, NULL );
		printf("cmd: pop\n");		
	}
	print_queue( queue );
	
	for( i=N; i<(N+N+N); ++i ) {
		cku_queue_push( queue, &i );
		print_queue(queue);
		printf("cmd: push\n");		
	}
	print_queue(queue);

	cku_queue_free( &queue );
}
