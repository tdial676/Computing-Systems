#include <assert.h>
#include <stdlib.h>

#include "queue.h"

const size_t MAX_QUEUE_SIZE = 100000;

int main() {
    queue_t *queue = queue_init();

    size_t push_idx = 0;
    size_t pop_idx = 0;

    while (push_idx < MAX_QUEUE_SIZE) {
        // Randomly enqueue a burst of elements.
        size_t push_burst_end = push_idx + rand() % 1000;
        if (push_burst_end > MAX_QUEUE_SIZE) {
            push_burst_end = MAX_QUEUE_SIZE;
        }
        while (push_idx < push_burst_end) {
            size_t *value = malloc(sizeof(size_t));
            assert(value != NULL);
            *value = push_idx;
            queue_enqueue(queue, value);
            push_idx++;
        }

        // Randomly dequeue a burst of elements, making sure to not
        // dequeue more than we're previously enqueued.
        size_t pop_burst_end = pop_idx + rand() % 1000;
        if (pop_burst_end >= push_burst_end) {
            pop_burst_end = push_burst_end;
        }
        while (pop_idx < pop_burst_end) {
            size_t *value = queue_dequeue(queue);
            assert(*value == pop_idx);
            free(value);
            pop_idx++;
        }
    }

    // If remaining elements are left on the queue, verify order.
    while (pop_idx < MAX_QUEUE_SIZE) {
        size_t *value = queue_dequeue(queue);
        assert(*value == pop_idx);
        free(value);
        pop_idx++;
    }

    queue_free(queue);
}
