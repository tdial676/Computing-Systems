/*
 * memlib.c - a module that simulates the memory system.  Needed because it
 *            allows us to interleave calls from the student's malloc package
 *            with the system's malloc package in libc.
 */
#include "memlib.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#define MAX_HEAP (100 * (1 << 20)) /* 100 MB */

/* private variables */
static uint8_t *heap;
static uint8_t *mem_brk;

/*
 * mem_init - initialize the memory system model
 */
void mem_init() {
    heap = mmap((void *) 0x800000000,        /* suggested start*/
                MAX_HEAP,                    /* length */
                PROT_READ | PROT_WRITE,      /* heap can be read or written */
                MAP_PRIVATE | MAP_ANONYMOUS, /* initialize region with zeros */
                -1,                          /* fd (unused) */
                0                            /* offset (unused) */
    );

    /* Heap is initially empty. */
    mem_reset_brk(true);
}

/*
 * mem_deinit - free the storage used by the memory system model
 */
void mem_deinit() {
    munmap(heap, MAX_HEAP);
}

/*
 * mem_reset_brk - reset the simulated brk pointer to make an empty heap
 */
void mem_reset_brk(bool clear) {
    mem_brk = heap;

    /* Fill heap with garbage since it is uninitialized. */
    if (clear) {
        memset(heap, 0xCC, MAX_HEAP);
    }
}

/*
 * mem_sbrk - simple model of the sbrk function. Extends the heap
 *    by incr bytes and returns the start address of the new area. In
 *    this model, the heap cannot be shrunk.
 */
void *mem_sbrk(ssize_t incr) {
    void *old_brk = mem_brk;

    if (incr < 0 || mem_brk + incr > heap + MAX_HEAP) {
        errno = ENOMEM;
        fprintf(stderr, "ERROR: mem_sbrk failed. Ran out of memory...\n");
        return (void *) -1;
    }

    mem_brk += incr;
    return old_brk;
}

/*
 * mem_heap_lo - return address of the first heap byte
 */
void *mem_heap_lo() {
    return heap;
}

/*
 * mem_heap_hi - return address of last heap byte
 */
void *mem_heap_hi() {
    return mem_brk - 1;
}

/*
 * mem_heapsize() - returns the heap size in bytes
 */
size_t mem_heapsize() {
    return mem_brk - heap;
}
