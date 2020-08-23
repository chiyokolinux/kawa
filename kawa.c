// #include <stdio.h>
// #include <unistd.h>
#include <string.h>

#include "config.h"

#include "help.h"
#include "install.h"
#include "repoadd.h"
#include "update.h"
#include "search.h"
#include "show.h"

int main(int argc, char *argv[]) {
    if (argc != 2 && argc != 3 && argc != 4) {
        return help()+1;
    }
    if (!strcmp(argv[1], "sync") && argc == 2)
        return sync_all();
    else if (!strcmp(argv[1], "install") && argc == 3)
        return install(argc, argv);
    else if (!strcmp(argv[1], "update") && argc == 2)
        return update();
    else if (!strcmp(argv[1], "search") && argc == 3)
        return search(argv[2]);
    else if (!strcmp(argv[1], "show") && argc == 3)
        return show(argv[2]);
    else if (!strcmp(argv[1], "help") && argc == 2)
        return help();
    else if (!strcmp(argv[1], "version") && argc == 2)
        return version();
    else if (!strcmp(argv[1], "addrepo") && argc == 4)
        return repoadd(argv[2], argv[3]);
    return help()+1;
}
