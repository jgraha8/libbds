/** @file
 *  @brief C generics queue data structures
 *
 *  @author Jason Graham <jgraha8@gmail.com>
 */

#ifndef __BDS_QUEUE_H__
#define __BDS_QUEUE_H__

#include <libbds/bds_modulus.h>
#include <stdbool.h>
#include <stdlib.h>

#define BDS_QUEUE_ISLINEAR(front, back) ((front) <= (back))

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Queue data structure
 *
 * Data are stored in a ring buffer.
 */
struct bds_queue {
        size_t n_alloc;            ///< Number of elements allocated in data vector @c v
        size_t n_elem;             ///< Number of elements currently in the queue
        size_t elem_len;           ///< Length in bytes of each data element
        bool auto_resize;          ///< Flag for enabling automatic resizing of the queue vector (false by default)
        size_t front;              ///< Index of element at the front of the queue
        void *v;                   ///< Address of ring buffer containing the data
        void (*elem_dtor)(void *); ///< Destructor for each data element in ring buffer
};

/**
 * @brief Constructor for the queue data structrue
 *
 * @param queue Address of queue object
 * @param n_alloc Number of elements to allocate in the data buffer
 * @param elem_len Length in bytes of each data element
 * @param elem_dtor Destructor for each data element (disabled if NULL)
 */
void bds_queue_ctor(struct bds_queue *queue, size_t n_alloc, size_t elem_len, void (*elem_dtor)(void *));

/**
 * @brief Destructor for the queue data structrue
 *
 * @param queue Address of queue object
 */
void bds_queue_dtor(struct bds_queue *queue);

/**
 * @brief Allocator for the queue data structrue
 *
 * @param n_alloc Number of elements to allocate in the data buffer
 * @param elem_len Length in bytes of each data element
 * @param elem_dtor Destructor for each data element (disabled if NULL)
 * @retval Address of queue object
 */
struct bds_queue *bds_queue_alloc(size_t n_alloc, size_t elem_len, void (*elem_dtor)(void *));

/**
 * @brief Frees the queue data structrue
 *
 * @param queue Address of queue object address (sets the queue object address to @c NULL)
 */
void bds_queue_free(struct bds_queue **queue);

/**
 * @brief Sets the value for the current value for the auto-resize flag
 *
 * @param queue Address of queue object
 * @param Value for the auto-resize flag (true is enable; false is disabled)
 */
static inline void bds_queue_set_autoresize(struct bds_queue *queue, bool auto_resize)
{
        queue->auto_resize = auto_resize;
}

/**
 * @brief Gets the current value of the auto-resize flag
 *
 * @param queue Address of queue object
 * @retval Current value for the auto-resize flag
 */
static inline bool bds_queue_get_autoresize(struct bds_queue *queue) { return queue->auto_resize; }

/**
 * @brief Tests if the queue is empty
 *
 * @param queue Address of queue object
 * @retval Returns true if the queue is empty; false otherwise
 */
static inline bool bds_queue_isempty(const struct bds_queue *queue) { return (queue->n_elem == 0); }

/**
 * @brief Tests if the queue is full
 *
 * @param queue Address of queue object
 * @retval Returns true if the queue is full; false otherwise
 */
static inline bool bds_queue_isfull(const struct bds_queue *queue) { return (queue->n_elem == queue->n_alloc); }

/**
 * @brief Resizes the queue using a doubling strategy
 *
 * In addition to manually resizing the queue, it can be used to
 * premptively resize the queue if the auto_resize option is set.
 *
 * @param queue Address of queue object
 *
 */
void bds_queue_resize(struct bds_queue *queue);

/**
 * @brief Provides the number of elements in the queue
 *
 * @param queue Address of queue object
 */
static inline size_t bds_queue_size(const struct bds_queue *queue) { return queue->n_elem; }

/**
 * @brief Pushes an element into the back of the queue
 *
 * @param queue Address of queue object
 * @param v Address of data element to enqueue
 * @retval Returns 0 upon successful push and non-zero otherwise (e.g. if queue is full and auto-resize is disabled; see @c bds_queue_set_autoresize)
 */
int bds_queue_push(struct bds_queue *queue, const void *v);

/**
 * @brief Pops an element from the front of the queue
 *
 * @param queue Address of queue object
 * @param v Address of a buffer to copy the popped data element (if @c NULL then the element is not copied)
 * @retval Address of the internal stack vector of popped data element (returns @c NULL if the queue is empty)
 */
void *bds_queue_pop(struct bds_queue *queue, void *v);

/**
 * @brief Clears the queue (does not deallocate memory)
 *
 * @param queue Address of queue object
 */
void bds_queue_clear(struct bds_queue *queue);

/**
 * @brief Clears n-elements from the front of the queue (does not deallocate memory)
 *
 * If @c n_clear is greater than or equal to the size of the queue, the entire is queue is cleared.
 *
 * @param queue Address of queue object
 * @param n_clear Number of elements to clear
 */
void bds_queue_clear_nfront(struct bds_queue *queue, size_t n_clear);

/**
 * @brief Gets the address of queue data vector
 *
 * @param queue Address of the queue object
 * @retval Address of the internal queue vector
 */
inline static const void *bds_queue_ptr(const struct bds_queue *queue) { return (const void *)queue->v; }

/**
 * @brief Gets the index of the front of the queue
 *
 * @param stack Address of the queue object
 * @retval Index of front element in the queue
 */
inline static size_t bds_queue_front(const struct bds_queue *queue) { return queue->front; }

/**
 * @brief Gets the address of front data element
 *
 * @param stack address of the stack object
 * @retval address of the internal queue vector of the front data element (returns @c NULL if the queue is
 * empty)
 */
inline static const void *bds_queue_frontptr(const struct bds_queue *queue)
{
        if (bds_queue_isempty(queue))
                return NULL;
        return (const void *)(queue->v + queue->front * queue->elem_len);
}

/**
 * @brief Gets the index of the back of the queue
 *
 * @param stack Address of the queue object
 * @retval Index of back element in the queue
 */
inline static size_t bds_queue_back(const struct bds_queue *queue)
{
        // return ( queue->front + queue->n_elem + queue->n_alloc - 1 ) % queue->n_alloc;
        return BDS_MOD(queue->front + queue->n_elem + queue->n_alloc - 1, queue->n_alloc);
}

/**
 * @brief Gets the address of back data element
 *
 * @param stack Address of the stack object
 * @retval Address of the internal queue vector of the back data element (returns @c NULL if the queue is empty)
 */
inline static const void *bds_queue_backptr(const struct bds_queue *queue)
{
        if (bds_queue_isempty(queue))
                return NULL;
        return (const void *)(queue->v + bds_queue_back(queue) * queue->elem_len);
}

/**
 * @brief Tests if all elements in the queue are linear
 *
 * @param queue Address of queue object
 * @retval Returns true if the queue is linear; false otherwise
 */
static inline bool bds_queue_islinear(struct bds_queue *queue)
{
        return BDS_QUEUE_ISLINEAR(queue->front, bds_queue_back(queue));
}

/**
 * @brief Linearizes the queue ring buffer
 *
 * This procedure ensures that all elements are contiguous starting
 * from @c front to @c back. The address of the queue data vector
 * returned by @c bds_queue_ptr is not modified.
 *
 * @param stack Address of the stack object
 */
void bds_queue_linearize(struct bds_queue *queue);

const void *bds_queue_lsearch(const struct bds_queue *queue, const void *key,
                              int (*compar)(const void *, const void *));

#ifdef __cplusplus
}
#endif

#endif // __BDS_QUEUE_H__
