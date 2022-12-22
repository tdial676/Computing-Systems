#include "util.h"

void do_access(page_t *pages) {
    force_read(pages[24]);
}
