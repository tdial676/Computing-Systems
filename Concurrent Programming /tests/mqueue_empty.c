#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "queue.h"

const size_t MAX_QUEUE_SIZE = 100000;
const size_t NUM_THREADS = 16;

queue_t *queue;

void *enqueue_worker(void *arg) {
    size_t thread_id = *(size_t *) arg;
    free(arg);

    for (size_t i = thread_id; i < MAX_QUEUE_SIZE; i += NUM_THREADS) {
        size_t *value = malloc(sizeof(size_t));
        assert(value != NULL);
        *value = i;
        queue_enqueue(queue, value);
    }

    return NULL;
}

void *dequeue_worker(void *_arg) {
    (void) _arg;
    size_t thread_indices[NUM_THREADS] = {0};

    for (size_t i = 0; i < MAX_QUEUE_SIZE; i++) {
        size_t *value = queue_dequeue(queue);

        size_t thread_id = *value % NUM_THREADS;
        size_t thread_index = thread_indices[thread_id]++;
        assert(*value == thread_index * NUM_THREADS + thread_id);

        free(value);
    }

    for (size_t i = 0; i < NUM_THREADS; i++) {
        assert(thread_indices[i] == MAX_QUEUE_SIZE / NUM_THREADS);
    }

    return NULL;
}

int main() {
    queue = queue_init();

    pthread_t enqueue_threads[NUM_THREADS];
    pthread_t dequeue_thread;

    int result = pthread_create(&dequeue_thread, NULL, dequeue_worker, NULL);
    assert(result == 0);
    sleep(5);

    for (size_t i = 0; i < NUM_THREADS; i++) {
        size_t *thread_id = malloc(sizeof(size_t));
        assert(thread_id != NULL);
        *thread_id = i;
        result = pthread_create(&enqueue_threads[i], NULL, enqueue_worker, thread_id);
        assert(result == 0);
    }

    for (size_t i = 0; i < NUM_THREADS; i++) {
        result = pthread_join(enqueue_threads[i], NULL);
        assert(result == 0);
    }
    result = pthread_join(dequeue_thread, NULL);
    assert(result == 0);

    queue_free(queue);
}
