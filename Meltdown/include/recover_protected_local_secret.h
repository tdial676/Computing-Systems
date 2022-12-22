#include <inttypes.h>
#include <stddef.h>

const char SECRET[] = "CACHE";

static inline void cache_secret(void) {
    volatile const char *secret = SECRET;
    while (*secret != '\0') {
        secret++;
    }
}

static inline uint8_t access_secret(size_t i) {
    *(volatile uint8_t *) NULL;
    return SECRET[i];
}
