/** @file
 *  @brief C generics queue data structures
 *
 *  @author Jason Graham <jgraha8@gmail.com>
 */

#ifndef __CKU_QUEUE_H__
#define __CKU_QUEUE_H__

#include <stdlib.h>
#include <stdbool.h>

/**
 * @brief Queue data structure
 *
 * Data are stored in a ring buffer.
 */
struct cku_queue {
	size_t n_alloc;  ///< Number of elements allocated in data vector @c v
	size_t n_elem;   ///< Number of elements currently in the queue  
	size_t elem_len;       ///< Length in bytes of each data element
        size_t front;    ///< Beginning element of the queue
	void *v;               ///< Ring buffer containing the data
};

__inline__
static const void *cku_queue_frontptr( const struct cku_queue *queue );

__inline__
static size_t cku_queue_back( const struct cku_queue *queue );

__inline__
static const void *cku_queue_backptr( const struct cku_queue *queue );

__inline__
static size_t cku_queue_end( const struct cku_queue *queue );

__inline__
static const void *cku_queue_endptr( const struct cku_queue *queue );


/**
 * @brief Constructor for the queue data structrue
 *
 * @param queue Queue data structure
 * @param n_elem Number of elements to allocate in the data buffer
 * @param elem_len Length in bytes of each data element
 */
void cku_queue_ctor( struct cku_queue *queue, size_t n_alloc, size_t elem_len );

/**
 * @brief Destructor for the queue data structrue
 *
 * @param queue Queue data structure
 */
void cku_queue_dtor( struct cku_queue *queue );

/**
 * @brief Allocator for the queue data structrue
 *
 * @param n_elem Number of elements to allocate in the data buffer
 * @param elem_len Length in bytes of each data element
 * @retval Queue data structure
 */
struct cku_queue *cku_queue_alloc( size_t n_alloc, size_t elem_len );

/**
 * @brief Frees the queue data structrue 
 *
 * @param queue Pointer to address of the queue data structure (nullified on exit)
 */
void cku_queue_free( struct cku_queue **queue );

/**
 * @brief Tests if the queue is empty
 *
 * @param queue Queue data structure
 */
__inline__
static bool cku_queue_isempty( const struct cku_queue *queue )
{
	return ( queue->n_elem == 0 );
}

__inline__
static bool cku_queue_isfull( const struct cku_queue *queue )
{
	return ( queue->n_elem == queue->n_alloc );
}


/**
 * @brief Pushes an element into the back of the queue
 *
 * @param queue Queue data structure
 * @param v Data element to enqueue
 */   
void cku_queue_push( struct cku_queue *queue, const void *v );

/**
 * @brief Pops an element from the front of the queue
 *
 * @param queue Queue data structure
 * @param v Data element removed from queue. If NULL, then only a pointer is returned.
 * @retval Returns pointer to popped element if pop is successful (NULL otherwise, e.g. if queue is already empty)
 */   
const void *cku_queue_pop( struct cku_queue *queue, void *v );

__inline__
static size_t cku_queue_front( const struct cku_queue *queue )
{
	return queue->front;
}

__inline__
static const void *cku_queue_frontptr( const struct cku_queue *queue )
{
	if( cku_queue_isempty( queue ) )
		return NULL;
	return (const void *)( queue->v + queue->front * queue->elem_len );
}

__inline__
static size_t cku_queue_back( const struct cku_queue *queue )
{
	return ( queue->front + queue->n_elem - 1 ) % queue->n_alloc;
}

__inline__
static const void *cku_queue_backptr( const struct cku_queue *queue )
{
	if( cku_queue_isempty( queue ) )
		return NULL;	
	return (const void *)( queue->v + cku_queue_back( queue ) * queue->elem_len );
}


void cku_queue_linearize( struct cku_queue *queue );

#endif // __CKU_QUEUE_H__
