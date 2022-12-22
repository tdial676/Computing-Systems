#ifndef _UTIL_H
#define _UTIL_H

#include <inttypes.h>
#include <x86intrin.h>

#define PAGE_SIZE 4096

typedef uint8_t page_t[PAGE_SIZE];

/**
 * Forces a memory read of the byte at the given address.
 * This will load the byte at the address into the cache.
 */
inline void force_read(const void *address) {
    *(volatile uint8_t *) address;
}

/**
 * Evicts any cache line currently storing the given address.
 * This ensures that the byte at the address is no longer in the cache.
 */
void flush_cache_line(const void *address) {
    _mm_clflush(address);
    _mm_mfence();
    for (volatile int i = 0; i < 10000; i++) {
    }
}

/**
 * Counts the number of processor clocks elapsed when reading a byte at the given address.
 * Note that the number of clocks can be quite large
 * if the process happens to be interrupted in this function.
 */
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

#endif /* _UTIL_H */
