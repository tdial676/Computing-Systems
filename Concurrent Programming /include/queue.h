#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct node {
    struct node *next;
    void *value;
} node_t;

typedef struct queue{
    node_t *head;
    node_t *tail;
    size_t size;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}queue_t;

/**
 * Creates a new heap-allocated FIFO queue. The queue is initially empty.
 *
 * @return a pointer to the new queue
 */
queue_t *queue_init(void);

/**
 * Enqueues a value into a queue. There is no maximum capacity,
 * so this should succeed unless the program runs out of memory.
 * This function should be concurrency-safe:
 * multiple threads may call queue_enqueue() or queue_dequeue() simultaneously.
 *
 * @param queue the queue to append to
 * @param value the value to add to the back of the queue
 */
void queue_enqueue(queue_t *queue, void *value);

/**
 * Dequeues a value from a queue.
 * The value returned is the first enqueued value that was not yet dequeued.
 * If the queue is empty, this thread should block until another thread enqueues a value.
 * This function should be concurrency-safe:
 * multiple threads may call queue_enqueue() or queue_dequeue() simultaneously.
 *
 * @param queue the queue to remove from
 * @return the value at the front of the queue
 */
void *queue_dequeue(queue_t *queue);

/**
 * Frees all resources associated with a heap-allocated queue.
 * You may assume that the queue is already empty.
 *
 * @param queue a queue returned from queue_init()
 */
void queue_free(queue_t *queue);

#endif /* QUEUE_H */
