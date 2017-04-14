/** @file
 *  @brief C generics queue data structures
 *
 *  @author Jason Graham <jgraha8@gmail.com>
 */

#ifndef __CKU_QUEUE_H__
#define __CKU_QUEUE_H__

#include <stdlib.h>
#include <stdbool.h>

#define CKU_QUEUE_ISLINEAR( front, back ) ( (front) <= (back) )

/**
 * @brief Queue data structure
 *
 * Data are stored in a ring buffer.
 */
struct cku_queue {
	size_t n_alloc;  ///< Number of elements allocated in data vector @c v
	size_t n_elem;   ///< Number of elements currently in the queue  
	size_t elem_len; ///< Length in bytes of each data element
	bool auto_resize; ///< Flag for enabling automatic resizing of the queue vector (false by default)
        size_t front;    ///< Index of element at the front of the queue
	void *v;         ///< Address of ring buffer containing the data
};


/**
 * @brief Constructor for the queue data structrue
 *
 * @param queue Address of queue object
 * @param n_alloc Number of elements to allocate in the data buffer
 * @param elem_len Length in bytes of each data element
 */
void cku_queue_ctor( struct cku_queue *queue, size_t n_alloc, size_t elem_len );

/**
 * @brief Destructor for the queue data structrue
 *
 * @param queue Address of queue object
 * @param elem_dtor Destructor for each data element (disabled if NULL)
 */
void cku_queue_dtor( struct cku_queue *queue, void (*elem_dtor)(void *) );

/**
 * @brief Allocator for the queue data structrue
 *
 * @param n_alloc Number of elements to allocate in the data buffer
 * @param elem_len Length in bytes of each data element
 * @retval Queue data structure
 */
struct cku_queue *cku_queue_alloc( size_t n_alloc, size_t elem_len );

/**
 * @brief Frees the queue data structrue 
 *
 * @param queue Address of queue object address (sets the queue object address to @c NULL)
 * @param elem_dtor Destructor for each data element (disabled if NULL) 
 */
void cku_queue_free( struct cku_queue **queue, void (*elem_dtor)(void *) );

/**
 * @brief Tests if the queue is empty
 *
 * @param queue Address of queue object
 * @retval Returns true if the queue is empty; false otherwise
 */
static inline bool cku_queue_isempty( const struct cku_queue *queue )
{
	return ( queue->n_elem == 0 );
}

/**
 * @brief Tests if the queue is full
 *
 * @param queue Address of queue object
 * @retval Returns true if the queue is full; false otherwise
 */
static inline bool cku_queue_isfull( const struct cku_queue *queue )
{
	return ( queue->n_elem == queue->n_alloc );
}

/**
 * @brief Resizes the queue using a doubling strategy
 *
 * In addition to manually resizing the queue, it can be used to
 * premptively resize the queue if the auto_resize option is set.
 *
 * @param queue Address of queue object
 *
 */ 
void cku_queue_resize( struct cku_queue *queue );

/**
 * @brief Provides the number of elements in the queue
 *
 * @param queue Address of queue object
 */
static inline size_t cku_queue_size( const struct cku_queue *queue )
{
	return queue->n_elem;
}

/**
 * @brief Pushes an element into the back of the queue
 *
 * @param queue Address of queue object
 * @param v Address of data element to enqueue
 */   
void cku_queue_push( struct cku_queue *queue, const void *v );

/**
 * @brief Pops an element from the front of the queue
 *
 * @param queue Address of queue object
 * @param v Address of a buffer to copy the popped data element (if @c NULL then the element is not copied)
 * @retval Address of the internal stack vector of popped data element (returns @c NULL if the queue is empty)
 */   
void *cku_queue_pop( struct cku_queue *queue, void *v );

/**
 * @brief Clears the queue (does not deallocate memory)
 *
 * @param queue Address of queue object
 * @param elem_dtor Destructor for each data element (disabled if NULL) 
 */
void cku_queue_clear( struct cku_queue *queue, void (*elem_dtor)(void *) );

/**
 * @brief Clears n-elements from the front of the queue (does not deallocate memory)
 *
 * If @c n_clear is greater than or equal to the size of the queue, the entire is queue is cleared.
 *
 * @param queue Address of queue object
 * @param n_clear Number of elements to clear
 * @param elem_dtor Destructor for each data element (disabled if NULL) 
 */
void cku_queue_clear_nfront( struct cku_queue *queue, size_t n_clear, void (*elem_dtor)(void *) );

/**
 * @brief Gets the address of queue data vector
 *
 * @param queue Address of the queue object
 * @retval Address of the internal queue vector
 */
inline static const void *cku_queue_ptr( const struct cku_queue *queue )
{
	return (const void *)queue->v;
}

/**
 * @brief Gets the index of the front of the queue
 *
 * @param stack Address of the queue object
 * @retval Index of front element in the queue
 */
inline static size_t cku_queue_front( const struct cku_queue *queue )
{
	return queue->front;
}

/**
 * @brief Gets the address of front data element
 *
 * @param stack address of the stack object
 * @retval address of the internal queue vector of the front data element (returns @c NULL if the queue is empty)
 */
inline static const void *cku_queue_frontptr( const struct cku_queue *queue )
{
	if( cku_queue_isempty( queue ) )
		return NULL;
	return (const void *)( queue->v + queue->front * queue->elem_len );
}

/**
 * @brief Gets the index of the back of the queue
 *
 * @param stack Address of the queue object
 * @retval Index of back element in the queue
 */
inline static size_t cku_queue_back( const struct cku_queue *queue )
{
	// Becareful with the subtraction of non-signed integral types
	return ( queue->front + queue->n_elem + queue->n_alloc - 1 ) % queue->n_alloc;
}

/**
 * @brief Gets the address of back data element
 *
 * @param stack Address of the stack object
 * @retval Address of the internal queue vector of the back data element (returns @c NULL if the queue is empty)
 */
inline static const void *cku_queue_backptr( const struct cku_queue *queue )
{
	if( cku_queue_isempty( queue ) )
		return NULL;	
	return (const void *)( queue->v + cku_queue_back( queue ) * queue->elem_len );
}

/**
 * @brief Tests if all elements in the queue are linear
 *
 * @param queue Address of queue object
 * @retval Returns true if the queue is linear; false otherwise
 */ 
static inline bool cku_queue_islinear( struct cku_queue *queue )
{
	return CKU_QUEUE_ISLINEAR( queue->front, cku_queue_back(queue) );
}


/**
 * @brief Linearizes the queue ring buffer
 *
 * This procedure ensures that all elements are contiguous starting
 * from @c front to @c back. The address of the queue data vector
 * returned by @c cku_queue_ptr is not modified.
 *
 * @param stack Address of the stack object
 */
void cku_queue_linearize( struct cku_queue *queue );

const void *cku_queue_lsearch( const struct cku_queue *queue, const void *key,
			       int (*compar)( const void *, const void *) );


#endif // __CKU_QUEUE_H__
