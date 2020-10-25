#include "error.h"

void malloc_fail() {
    perror("malloc");
    exit(-1);
}
