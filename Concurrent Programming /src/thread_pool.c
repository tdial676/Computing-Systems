#include "thread_pool.h"

#include <assert.h>
#include <stdlib.h>

typedef struct func_aux {
    work_function_t function;
    void *aux;
} func_aux_t;

void *thread_function(void *queue) {
    func_aux_t *func_aux = queue_dequeue((queue_t *) queue);
    while (func_aux != NULL) {
        func_aux->function(func_aux->aux);
        free(func_aux);
        func_aux = queue_dequeue(queue);
    }
    return NULL;
}

thread_pool_t *thread_pool_init(size_t num_worker_threads) {
    thread_pool_t *thread_pool = malloc(sizeof(thread_pool_t));
    assert(thread_pool != NULL);
    thread_pool->work = queue_init();
    thread_pool->threads_arr = malloc(sizeof(pthread_t) * num_worker_threads);
    assert(thread_pool->threads_arr != NULL);
    thread_pool->num_worker_threads = num_worker_threads;

    for (size_t i = 0; i < num_worker_threads; i++) {
        assert(pthread_create(&thread_pool->threads_arr[i], NULL, thread_function,
                              (void *) thread_pool->work) == 0);
    }
    return thread_pool;
}
void thread_pool_add_work(thread_pool_t *pool, work_function_t function, void *aux) {
    func_aux_t *func_aux = malloc(sizeof(func_aux_t));
    assert(func_aux != NULL);
    func_aux->aux = aux;
    func_aux->function = function;
    queue_enqueue(pool->work, func_aux);
}

void thread_pool_finish(thread_pool_t *pool) {
    for (size_t i = 0; i < pool->num_worker_threads; i++) {
        queue_enqueue(pool->work, NULL);
    }
    for (size_t i = 0; i < pool->num_worker_threads; i++) {
        assert(pthread_join(pool->threads_arr[i], NULL) == 0);
    }
    queue_free(pool->work);
    free(pool->threads_arr);
    free(pool);
}