#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

void mem_init(void);
void mem_deinit(void);
void *mem_sbrk(ssize_t incr);
void mem_reset_brk(bool clear);
void *mem_heap_lo(void);
void *mem_heap_hi(void);
size_t mem_heapsize(void);
