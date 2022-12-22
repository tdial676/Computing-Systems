#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <stddef.h>
#include "queue.h"
#include <pthread.h>

/**
 * A pool of threads which perform work in parallel.
 * The pool contains a fixed number of threads specified in thread_pool_init()
 * and a shared queue of work for the worker threads to run.
 * Each worker thread dequeues new work from the queue when its current work is finished.
 */
typedef struct thread_pool{
    queue_t *work;
    size_t num_worker_threads;
    pthread_t *threads_arr;
} thread_pool_t;

/** A function that can run on a thread in a thread pool */
typedef void (*work_function_t)(void *aux);

/**
 * Creates a new heap-allocated thread pool with the given number of worker threads.
 * All worker threads should start immediately so they can perform work
 * as soon as thread_pool_add_work() is called.
 *
 * @param num_worker_threads the number of threads in the pool
 * @return a pointer to the new thread pool
 */
thread_pool_t *thread_pool_init(size_t num_worker_threads);

/**
 * Adds work to a thread pool.
 * The work will be performed by a worker thread as soon as all previous work is finished.
 *
 * @param pool the thread pool to perform the work
 * @param function the function to call on a thread in the thread pool
 * @param aux the argument to call the work function with
 */
void thread_pool_add_work(thread_pool_t *pool, work_function_t function, void *aux);

/**
 * Waits for all work added to a thread pool to finish,
 * then frees all resources associated with a heap-allocated thread pool.
 * A special value (e.g. NULL) can be put in the work queue to mark the end of the work.
 * thread_pool_add_work() cannot be used on this pool once this function is called.
 *
 * @param pool the thread pool to close
 */
void thread_pool_finish(thread_pool_t *pool);

#endif /* THREAD_POOL_H */
