// #include <stdio.h>
// #include <unistd.h>
#include <string.h>

#include "config.h"

#include "update.h"
#include "show.h"

int main(int argc, char *argv[]) {
    if (argc != 2 && argc != 3) {
        printf("-- TODO: INSERT USAGE/HELP THING HERE --\n");
        return 1;
    }
    if (!strcmp(argv[1], "sync") && argc == 2)
        return sync_all();
    else if (!strcmp(argv[1], "update") && argc == 2)
        return update();
    else if (!strcmp(argv[1], "show") && argc == 3)
        return show(argv[2]);
    printf("-- TODO: INSERT USAGE/HELP THING HERE --\n");
    return 1;
}
