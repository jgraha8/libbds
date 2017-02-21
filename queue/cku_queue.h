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
	unsigned int n_alloc;  ///< Number of elements allocated in data vector @c v
	unsigned int n_elem;   ///< Number of elements currently in the queue  
	size_t elem_len; ///< Length in bytes of each data element
        unsigned int i_front;     ///< Beginning element of the queue
	unsigned int i_back;	 ///< Ending index of the queue (first empty element)
	void *v;         ///< Ring buffer containing the data
};


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
bool cku_queue_isempty( const struct cku_queue *queue );

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

const void *cku_queue_end( struct cku_queue *queue, void *v );

__inline__
static const void *cku_queue_front( const struct cku_queue *queue )
{
	return (const void *)( queue->v + queue->i_front * queue->elem_len );
}

__inline__
static const void *cku_queue_back( const struct cku_queue *queue )
{
	return (const void *)( queue->v + queue->i_back * queue->elem_len );
}

__inline__
static unsigned int cku_queue_iend( const struct cku_queue *queue )
{
	return ( ( queue->i_back + 1 ) % queue->n_alloc );
}

__inline__
static const void *cku_queue_end( const struct cku_queue *queue )
{
	return (const void *)( queue->v + cku_queue_iend( queue ) * queue->elem_len );
}

#endif // __CKU_QUEUE_H__
