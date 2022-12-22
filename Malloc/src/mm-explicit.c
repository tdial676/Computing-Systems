/*
 * mm-explicit.c - The best malloc package EVAR!
 *
 * TODO (bug): Uh..this is an implicit list???
 */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memlib.h"
#include "mm.h"

/** The required alignment of heap payloads */
const size_t ALIGNMENT = 2 * sizeof(size_t);

/** The layout of each block allocated on the heap */
typedef struct {
    /** The size of the block and whether it is allocated (stored in the low bit) */
    size_t header;
    /**
     * We don't know what the size of the payload will be, so we will
     * declare it as a zero-length array.  This allow us to obtain a
     * pointer to the start of the payload.
     */
    uint8_t payload[];
} block_t;

typedef struct node {
    struct node *next;
    struct node *prev;
} node_t;

// The head node
static node_t *head = NULL;

void add_node(node_t *node) {
    if (head == NULL) {
        head = node;
        head->next = NULL;
        head->prev = NULL;
    }
    else {
        node->next = head;
        head->prev = node;
        node->prev = NULL;
        head = node;
    }
}

void delete_node(node_t *node) {
    if (node == head) {
        head = head->next;
    }
    else if (node->next == NULL) {
        node->prev->next = node->next;
    }

    else {
        node->next->prev = node->prev;
        node->prev->next = node->next;
    }
}
/**
 *  The first and last blocks on the heap */
static block_t *mm_heap_first = NULL;
static block_t *mm_heap_last = NULL;

/** Rounds up `size` to the nearest multiple of `n` */
static size_t round_up(size_t size, size_t n) {
    return (size + (n - 1)) / n * n;
}

/** Set's a block's header with the given size and allocation state */
static void set_header(block_t *block, size_t size, bool is_allocated) {
    block->header = size | is_allocated;
    block = (void *) block + size - sizeof(size_t);
    *(size_t *) block = size | is_allocated;
}

/** Extracts a block's size from its header */
static size_t get_size(block_t *block) {
    return block->header & ~1;
}

/** Extracts a block's allocation state from its header */
static bool is_allocated(block_t *block) {
    return block->header & 1;
}

/**
 * Finds the first free block in the heap with at least the given size.
 * If no block is large enough, returns NULL.
 */
static block_t *find_fit(size_t size) {
    node_t *node = head;
    while (node != NULL) {
        block_t *block = (block_t *) ((void *) node - sizeof(size_t));
        size_t cur_size = get_size(block);
        if (cur_size >= size) {
            // Block Splitting
            if (cur_size >= (size + sizeof(block_t) + sizeof(node_t) + sizeof(size_t))) {
                node_t *new_node = (void *) block + size + sizeof(size_t);
                add_node(new_node);
                set_header(block, size, true);
                set_header((void *) block + size, cur_size - size, false);
                if (block == mm_heap_last) {
                    mm_heap_last = (void *) block + size;
                }
            }
            delete_node(node);
            return block;
        }
        node = node->next;
    }
    return NULL;
}

/** Gets the header corresponding to a given payload pointer */
static block_t *block_from_payload(void *ptr) {
    return ptr - offsetof(block_t, payload);
}

/**
 * mm_init - Initializes the allocator state
 */
bool mm_init(void) {
    // We want the first payload to start at ALIGNMENT bytes from the start of the heap
    void *padding = mem_sbrk(ALIGNMENT - sizeof(block_t));
    if (padding == (void *) -1) {
        return false;
    }

    // Initialize the heap with no blocks
    mm_heap_first = NULL;
    mm_heap_last = NULL;
    head = NULL;
    return true;
}

/**
 * mm_malloc - Allocates a block with the given size
 */
size_t max(size_t num1, size_t num2) {
    return (num1 > num2) ? num1 : num2;
}

void *mm_malloc(size_t size) {
    // The block must have enough space for a header and be 16-byte aligned
    size =
        round_up(max(size, sizeof(node_t)) + sizeof(block_t) + sizeof(size_t), ALIGNMENT);

    // If there is a large enough free block, use it
    block_t *block = find_fit(size);
    if (block != NULL) {
        set_header(block, get_size(block), true);
        return block->payload;
    }

    // Otherwise, a new block needs to be allocated at the end of the heap
    block = mem_sbrk(size);
    if (block == (void *) -1) {
        return NULL;
    }

    // Update mm_heap_first and mm_heap_last since we extended the heap
    if (mm_heap_first == NULL) {
        mm_heap_first = block;
    }
    mm_heap_last = block;

    // Initialize the block with the allocated size
    set_header(block, size, true);
    return block->payload;
}

/**
 * mm_free - Releases a block to be reused for future allocations
 */
void mm_free(void *ptr) {
    // mm_free(NULL) does nothing
    if (ptr == NULL) {
        return;
    }

    // Mark the block as unallocated
    block_t *block = block_from_payload(ptr);
    size_t block_size_curr = get_size(block);
    set_header(block, block_size_curr, false);
    node_t *node = (node_t *) ((void *) block + sizeof(size_t));
    add_node(node);

    if (block < mm_heap_last) {
        block_t *next_block = (void *) block + get_size(block);
        if (!is_allocated(next_block)) {
            node_t *next_node = (void *) next_block + sizeof(size_t);
            size_t block_size_next = get_size(next_block);
            set_header(block, get_size(block) + block_size_next, false);
            delete_node(next_node);
            if (next_block == mm_heap_last) {
                mm_heap_last = block;
            }
        }
    }
    // Coalesscing Left by going to prev and coalescing right
    if (block > mm_heap_first) {
        block_t *block_footer = ((void *) block - sizeof(size_t));
        block_t *prev_block = (void *) block - get_size(block_footer);
        if (!is_allocated(prev_block)) {
            node_t *node = (node_t *) ((void *) block + sizeof(size_t));
            size_t block_size_prev = get_size(prev_block);
            set_header(prev_block, get_size(block) + block_size_prev, false);
            delete_node(node);
            if (block == mm_heap_last) {
                mm_heap_last = prev_block;
            }
        }
    }
}

/**
 * mm_realloc - Change the size of the block by mm_mallocing a new block,
 *      copying its data, and mm_freeing the old block.
 */
void *mm_realloc(void *old_ptr, size_t size) {
    if (size == 0) {
        mm_free(old_ptr);
        return NULL;
    }
    void *new_ptr = mm_malloc(size);
    if (old_ptr == NULL) {
        return new_ptr;
    }
    else {
        size_t block_size =
            get_size(block_from_payload(old_ptr)) - sizeof(size_t) - sizeof(block_t);
        if (size >= block_size) {
            memcpy(new_ptr, old_ptr, block_size);
        }
        else {
            memcpy(new_ptr, old_ptr, size);
        }
        mm_free(old_ptr);
        return new_ptr;
    }
}

/**
 * mm_calloc - Allocate the block and set it to zero.
 */
void *mm_calloc(size_t nmemb, size_t size) {
    if (size == 0 || nmemb == 0) {
        return NULL;
    }
    void *new_ptr = mm_malloc(nmemb * size);
    memset(new_ptr, 0, nmemb * size);
    return new_ptr;
}

/**
 * mm_checkheap - So simple, it doesn't need a checker!
 */
void mm_checkheap(void) {
}
