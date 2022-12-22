#ifndef MM_H
#define MM_H

#include <stdbool.h>
#include <stddef.h>

bool mm_init(void);
void *mm_malloc(size_t size);
void mm_free(void *ptr);
void *mm_realloc(void *ptr, size_t size);
void *mm_calloc(size_t nmemb, size_t size);
void mm_checkheap(void);

#endif /* MM_H */
