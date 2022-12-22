#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "thread_pool.h"

const time_t NS_PER_SEC = 1e9;
const size_t NUM_SLEEPS = 10;

void usage(char *argv[]) {
    fprintf(stderr, "Usage: %s <# of threads>\n", argv[0]);
    exit(1);
}

void sleeper(void *aux) {
    (void) aux;
    sleep(1);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usage(argv);
    }

    size_t num_threads = strtoul(argv[1], NULL, 0);
    if (num_threads == 0) {
        usage(argv);
    }

    struct timespec start;
    int result = clock_gettime(CLOCK_REALTIME, &start);
    assert(result == 0);

    thread_pool_t *pool = thread_pool_init(num_threads);

    for (size_t i = 0; i < NUM_SLEEPS; i++) {
        thread_pool_add_work(pool, sleeper, NULL);
    }

    thread_pool_finish(pool);

    struct timespec end;
    result = clock_gettime(CLOCK_REALTIME, &end);
    assert(result == 0);
    time_t duration = end.tv_sec - start.tv_sec +
                      (end.tv_nsec - start.tv_nsec) / NS_PER_SEC;
    printf("%lu\n", duration);
}
