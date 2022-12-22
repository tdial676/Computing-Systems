#include <inttypes.h>
#include <stddef.h>

static inline uint8_t access_secret(size_t i) {
    return "CLOCK"[i];
}
