#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "thread_pool.h"

const uint64_t MAX_CANDIDATE = 100000;
const size_t NUM_THREADS_PER_POOL = 8;
const size_t NUM_THREADPOOLS = 10;

void check_prime(void *_n) {
    uint64_t n = *(uint64_t *) _n;
    free(_n);

    for (uint64_t k = n / 2; k > 1; k--) {
        if (n % k == 0) {
            return;
        }
    }
    printf("%" PRIu64 "\n", n);
}

int main() {
    thread_pool_t *pools[NUM_THREADPOOLS];

    for (size_t i = 0; i < NUM_THREADPOOLS; i++) {
        pools[i] = thread_pool_init(NUM_THREADS_PER_POOL);
    }

    for (uint64_t i = 2; i < MAX_CANDIDATE; i++) {
        uint64_t *aux = malloc(sizeof(uint64_t));
        assert(aux != NULL);
        *aux = i;
        thread_pool_add_work(pools[i % NUM_THREADPOOLS], check_prime, aux);
    }

    for (size_t i = 0; i < NUM_THREADPOOLS; i++) {
        thread_pool_finish(pools[i]);
    }
}
