#include <assert.h>
#include <stdlib.h>

#include "queue.h"

const size_t MAX_QUEUE_SIZE = 100000;

int main() {
    queue_t *queue = queue_init();

    for (size_t i = 0; i < MAX_QUEUE_SIZE; i++) {
        size_t *value = malloc(sizeof(size_t));
        assert(value != NULL);
        *value = i;
        queue_enqueue(queue, value);
    }

    for (size_t i = 0; i < MAX_QUEUE_SIZE; i++) {
        size_t *value = queue_dequeue(queue);
        assert(*value == i);
        free(value);
    }

    queue_free(queue);
}
