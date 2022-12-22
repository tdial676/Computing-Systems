#include "util.h"

#include <x86intrin.h>

void flush_cache_line(const void *address) {
    _mm_clflush(address);
    _mm_mfence();
    for (volatile int i = 0; i < 10000; i++) {
    }
}

uint64_t time_read(const void *address) {
    uint64_t start = __rdtsc();
    _mm_lfence();
    force_read(address);
    _mm_mfence();
    _mm_lfence();
    uint64_t result = __rdtsc() - start;
    _mm_mfence();
    _mm_lfence();
    return result;
}
