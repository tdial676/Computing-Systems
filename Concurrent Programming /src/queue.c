#include "queue.h"

#include <assert.h>

queue_t *queue_init(void) {
    queue_t *queue = malloc(sizeof(queue_t));
    assert(queue != NULL);
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    assert(pthread_cond_init(&queue->cond, NULL) == 0);
    assert(pthread_mutex_init(&queue->mutex, NULL) == 0);
    return queue;
}

void queue_enqueue(queue_t *queue, void *value) {
    assert(pthread_mutex_lock(&queue->mutex) == 0);
    node_t *new_node = malloc(sizeof(node_t));
    assert(new_node != NULL);
    new_node->value = value;
    new_node->next = NULL;
    if (queue->size == 0) {
        queue->head = new_node;
        queue->tail = new_node;
    }
    else {
        queue->tail->next = new_node;
        queue->tail = new_node;
    }
    queue->size++;
    assert(pthread_cond_signal(&queue->cond) == 0);
    assert(pthread_mutex_unlock(&queue->mutex) == 0);
}

void *queue_dequeue(queue_t *queue) {
    assert(pthread_mutex_lock(&queue->mutex) == 0);
    while (queue->size == 0) {
        assert(pthread_cond_wait(&queue->cond, &queue->mutex) == 0);
    }
    node_t *curr = queue->head;
    void *removed = curr->value;
    queue->head = queue->head->next;
    if (queue->size == 1) {
        queue->tail = NULL;
    }
    queue->size--;
    free(curr);
    assert(pthread_mutex_unlock(&queue->mutex) == 0);
    return removed;
}

void queue_free(queue_t *queue) {
    free(queue);
}
