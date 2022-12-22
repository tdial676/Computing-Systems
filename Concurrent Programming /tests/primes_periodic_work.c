#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "thread_pool.h"

const uint64_t MAX_CANDIDATE = 100000;
const size_t NUM_THREADS = 16;
const struct timespec WORK_DELAY = {.tv_sec = 0, .tv_nsec = 100000};

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
    thread_pool_t *pool = thread_pool_init(NUM_THREADS);

    for (uint64_t i = 2; i < MAX_CANDIDATE; i++) {
        uint64_t *aux = malloc(sizeof(uint64_t));
        assert(aux != NULL);
        *aux = i;
        thread_pool_add_work(pool, check_prime, aux);
        int result = nanosleep(&WORK_DELAY, NULL);
        assert(result == 0);
    }

    thread_pool_finish(pool);
}
