#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "queue.h"

const size_t MAX_QUEUE_SIZE = 100000;
const size_t NUM_PUSH_THREADS = 8;
const size_t NUM_POP_THREADS = 8;

queue_t *queue;
size_t num_seen;
pthread_mutex_t num_seen_mutex;

void *enqueue_worker(void *arg) {
    size_t thread_id = *(size_t *) arg;
    free(arg);

    for (size_t i = thread_id; i < MAX_QUEUE_SIZE; i += NUM_PUSH_THREADS) {
        size_t *value = malloc(sizeof(size_t));
        assert(value != NULL);
        *value = i;
        queue_enqueue(queue, value);
    }

    return NULL;
}

void *dequeue_worker(void *_arg) {
    (void) _arg;
    for (size_t i = 0; i < MAX_QUEUE_SIZE; i++) {
        pthread_mutex_lock(&num_seen_mutex);
        if (num_seen == MAX_QUEUE_SIZE) {
            pthread_mutex_unlock(&num_seen_mutex);
            break;
        }
        num_seen++;
        pthread_mutex_unlock(&num_seen_mutex);

        size_t *value = queue_dequeue(queue);
        printf("%zu\n", *value);
        free(value);
    }
    return NULL;
}

int main() {
    queue = queue_init();
    num_seen = 0;
    int result = pthread_mutex_init(&num_seen_mutex, NULL);
    assert(result == 0);

    pthread_t enqueue_threads[NUM_PUSH_THREADS];
    pthread_t dequeue_threads[NUM_POP_THREADS];

    for (size_t i = 0; i < NUM_PUSH_THREADS; i++) {
        size_t *thread_id = malloc(sizeof(size_t));
        assert(thread_id != NULL);
        *thread_id = i;
        result = pthread_create(&enqueue_threads[i], NULL, enqueue_worker, thread_id);
        assert(result == 0);
    }
    for (size_t i = 0; i < NUM_POP_THREADS; i++) {
        result = pthread_create(&dequeue_threads[i], NULL, dequeue_worker, NULL);
        assert(result == 0);
    }

    for (size_t i = 0; i < NUM_PUSH_THREADS; i++) {
        result = pthread_join(enqueue_threads[i], NULL);
        assert(result == 0);
    }
    for (size_t i = 0; i < NUM_POP_THREADS; i++) {
        result = pthread_join(dequeue_threads[i], NULL);
        assert(result == 0);
    }

    result = pthread_mutex_destroy(&num_seen_mutex);
    assert(result == 0);

    queue_free(queue);
}
