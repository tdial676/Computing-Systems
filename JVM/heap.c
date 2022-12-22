#include "heap.h"

#include <stdlib.h>

typedef struct heap {
    /** Generic array of pointers. */
    int32_t **ptr;
    /** How many pointers there are currently in the array. */
    int32_t count;
} heap_t;

heap_t *heap_init() {
    // The heap array is initially allocated to hold zero elements.
    heap_t *heap = malloc(sizeof(heap_t));
    heap->ptr = NULL;
    heap->count = 0;
    return heap;
}

int32_t heap_add(heap_t *heap, int32_t *ptr) {
    heap->ptr = realloc(heap->ptr, (heap->count + 1) * sizeof(int32_t *));
    heap->ptr[heap->count] = ptr;
    int32_t temp = heap->count;
    heap->count += 1;
    return temp;
}

int32_t *heap_get(heap_t *heap, int32_t ref) {
    return heap->ptr[ref];
}

void heap_free(heap_t *heap) {
    for (int32_t i = 0; i < heap->count; i++) {
        free(heap->ptr[i]);
    }
    free(heap->ptr);
    free(heap);
}