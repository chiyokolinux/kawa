#include <stdio.h>
#include <unistd.h>

#include "config.h"

#include "update.h"

int main(int argc, char *argv[]) {
    if (getuid()) {
        printf("Because error handling is somewhere on TODO, just run this as root for now and everything should be fine.\n");
        return -1;
    }
    printf("wip\n");
    return update();
}
