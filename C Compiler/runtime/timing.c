#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// The number of seconds in a nanosecond
const double SEC_PER_NS = 1e-9;
// The prefix of the executable name this will be compiled into
const char TIME_EXECUTABLE_PREFIX[] = "bin/time-";

// basic_main() is the assembly function produced by the compiler
void basic_main(void);

double time_main(void) {
    // Compute the amount of CPU time used by basic_main()
    struct timespec start, end;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    basic_main();
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    // Compute the duration in seconds
    return end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) * SEC_PER_NS;
}

int main(int argc, char *argv[]) {
    assert(argc > 0);
    assert(strncmp(argv[0], TIME_EXECUTABLE_PREFIX, strlen(TIME_EXECUTABLE_PREFIX)) == 0);
    char *test_name = argv[0] + strlen(TIME_EXECUTABLE_PREFIX);

    // Rerun basic_main() for at least a second and at least 3 times
    double duration_sum = 0;
    double duration_log_sum = 0, duration_log_square_sum = 0;
    size_t runs = 0;
    while (duration_sum < 1.0 || runs < 3) {
        double duration = time_main();
        duration_sum += duration;
        double log_duration = log(duration);
        duration_log_sum += log_duration;
        duration_log_square_sum += log_duration * log_duration;
        runs++;
    }

    /* Compute the mean and standard deviation of the estimated time basic_main() takes
     * We use log(duration), so we compute the geometric mean and variance.
     * This makes the mean much more resistant to large outliers. */
    double mean_log_duration = duration_log_sum / runs;
    double variance_log_duration =
        (duration_log_square_sum / runs - mean_log_duration * mean_log_duration) / runs;
    printf(
        "test_name,mean_log_duration,variance_log_duration\n"
        "%s,%f,%e\n",
        test_name, mean_log_duration, variance_log_duration);
    fprintf(stderr, "%s mean duration: %e seconds (+/- %e x)\n", test_name,
            exp(mean_log_duration), expm1(sqrt(variance_log_duration)));
}
