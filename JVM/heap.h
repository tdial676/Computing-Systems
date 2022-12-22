#ifndef HEAP_H
#define HEAP_H

#include <inttypes.h>

/**
 * Represents the array of pointers to heap-allocated int32_t arrays.
 */
typedef struct heap heap_t;

/**
 * Initializes a heap. The capacity of this heap is initially zero.
 */
heap_t *heap_init();

/**
 * Add a pointer to the heap and get a reference. For simplification, the
 * reference is an index into a generic heap-allocated array.
 *
 * @param ptr Pointer of an int32_t array to add to the heap.
 * @returns A "reference" to the pointer.
 */
int32_t heap_add(heap_t *heap, int32_t *ptr);

/**
 * Retrieve a pointer from the heap.
 *
 * @param ref A "reference".
 * @returns A pointer to an int32_t array from the heap.
 */
int32_t *heap_get(heap_t *heap, int32_t ref);

/**
 * Frees elements of the heap-allocated int32_t arrays.
 *
 * @param heap array of heap-allocated pointers
 */
void heap_free(heap_t *heap);

#endif