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
	size_t n_elem;   ///< Number of elements allocated in data vector @c v
	size_t elem_len; ///< Length in bytes of each data element
        size_t i_begin;  ///< Beginning index of the queue
	size_t i_end;	 ///< Ending index of the queue (first empty element)
	void *v;         ///< Ring buffer containing the data
};


/**
 * @brief Constructor for the queue data structrue
 *
 * @param queue Queue data structure
 * @param n_elem Number of elements to allocate in the data buffer
 * @param elem_len Length in bytes of each data element
 */
void cku_queue_ctor( struct cku_queue *queue, size_t n_elem, size_t elem_len );

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
struct cku_queue *cku_queue_alloc( size_t n_elem, size_t elem_len );

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
 * @param v Data element removed from queue
 * @retval Returns 0 if pop is successful (non-zero otherwise, e.g. if queue is already empty)
 */   
int cku_queue_pop( struct cku_queue *queue, void *v );



#endif // __CKU_QUEUE_H__
