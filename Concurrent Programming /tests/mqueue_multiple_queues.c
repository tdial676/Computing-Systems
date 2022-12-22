#include <assert.h>
#include <pthread.h>
#include <time.h>

#include "queue.h"

const size_t QUEUES = 10;
const size_t QUEUE_VALUES = 10;
const size_t REPETITIONS = 100;
const struct timespec ENQUEUE_DELAY = {.tv_sec = 0, .tv_nsec = 1000000};

queue_t *queues[QUEUES];

void *enqueuer(void *_queue_index) {
    size_t queue_index = (size_t) _queue_index;
    queue_t *queue = queues[queue_index];
    for (size_t i = 0; i < QUEUE_VALUES; i++) {
        size_t value = queue_index + i * QUEUES;
        queue_enqueue(queue, (void *) value);
        int result = nanosleep(&ENQUEUE_DELAY, NULL);
        assert(result == 0);
    }
    return NULL;
}

void *dequeuer(void *_queue_index) {
    size_t queue_index = (size_t) _queue_index;
    queue_t *queue = queues[queue_index];
    for (size_t i = 0; i < QUEUE_VALUES; i++) {
        size_t value = (size_t) queue_dequeue(queue);
        assert(value == queue_index + i * QUEUES);
    }
    return NULL;
}

int main() {
    for (size_t repetition = 0; repetition < REPETITIONS; repetition++) {
        pthread_t enqueue_threads[QUEUES];
        pthread_t dequeue_threads[QUEUES];
        for (size_t i = 0; i < QUEUES; i++) {
            queues[i] = queue_init();
            int result = pthread_create(&dequeue_threads[i], NULL, dequeuer, (void *) i);
            assert(result == 0);
            result = pthread_create(&enqueue_threads[i], NULL, enqueuer, (void *) i);
            assert(result == 0);
        }
        for (size_t i = 0; i < QUEUES; i++) {
            int result = pthread_join(enqueue_threads[i], NULL);
            assert(result == 0);
            result = pthread_join(dequeue_threads[i], NULL);
            assert(result == 0);
            queue_free(queues[i]);
        }
    }
}
