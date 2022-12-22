#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "thread_pool.h"

const uint64_t MAX_CANDIDATE = 100000;

void usage(char *argv[]) {
    fprintf(stderr, "Usage: %s <# of threads>\n", argv[0]);
    exit(1);
}

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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usage(argv);
    }

    size_t num_threads = strtoul(argv[1], NULL, 0);
    if (num_threads == 0) {
        usage(argv);
    }

    thread_pool_t *pool = thread_pool_init(num_threads);

    for (uint64_t i = 2; i < MAX_CANDIDATE; i++) {
        uint64_t *aux = malloc(sizeof(uint64_t));
        assert(aux != NULL);
        *aux = i;
        thread_pool_add_work(pool, check_prime, aux);
    }

    thread_pool_finish(pool);
}
