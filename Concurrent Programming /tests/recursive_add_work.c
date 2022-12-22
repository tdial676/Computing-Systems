#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "thread_pool.h"

const size_t NUM_THREADS = 16;
const size_t MAX_DEPTH = 5;
const size_t BRANCH_FACTOR = 5;

thread_pool_t *pool;
size_t remaining_work;
pthread_mutex_t remaining_work_mutex;
pthread_cond_t done_cond;

void recursive_work(void *);

void add_work(size_t depth) {
    size_t *aux = malloc(sizeof(size_t));
    assert(aux != NULL);
    *aux = depth;
    thread_pool_add_work(pool, recursive_work, aux);
    int result = pthread_mutex_lock(&remaining_work_mutex);
    assert(result == 0);
    remaining_work++;
    result = pthread_mutex_unlock(&remaining_work_mutex);
    assert(result == 0);
}

void recursive_work(void *_depth) {
    size_t depth = *(size_t *) _depth;
    free(_depth);

    if (depth == MAX_DEPTH) {
        printf("Recursion depth hit!\n");
    }
    else {
        for (size_t i = 0; i < BRANCH_FACTOR; i++) {
            add_work(depth + 1);
        }
    }

    int result = pthread_mutex_lock(&remaining_work_mutex);
    assert(result == 0);
    if (--remaining_work == 0) {
        pthread_cond_signal(&done_cond);
    }
    result = pthread_mutex_unlock(&remaining_work_mutex);
    assert(result == 0);
}

int main() {
    pool = thread_pool_init(NUM_THREADS);
    remaining_work = 0;
    int result = pthread_mutex_init(&remaining_work_mutex, NULL);
    assert(result == 0);
    result = pthread_cond_init(&done_cond, NULL);
    assert(result == 0);

    add_work(0);
    result = pthread_mutex_lock(&remaining_work_mutex);
    assert(result == 0);
    while (remaining_work > 0) {
        result = pthread_cond_wait(&done_cond, &remaining_work_mutex);
        assert(result == 0);
    }
    result = pthread_mutex_unlock(&remaining_work_mutex);
    assert(result == 0);

    thread_pool_finish(pool);
    result = pthread_mutex_destroy(&remaining_work_mutex);
    assert(result == 0);
    result = pthread_cond_destroy(&done_cond);
    assert(result == 0);
}
