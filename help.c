#include "help.h"

int help() {
    printf("kawa - help\n");
    printf("-----------\n\n");
    printf("basic commands:\n");
    printf("    kawa install [package(s)]\n");
    printf("    kawa remove [package(s)]\n");
    printf("    kawa update\n");
    printf("    kawa search [term]\n");
    printf("    kawa show [package]\n");
    printf("    kawa sync\n");
    printf("    kawa cacheclean\n");
    printf("    kawa help\n");
    printf("    kawa version\n");
    printf("    kawa makepackages\n");
    printf("\ncommand options:\n");
    printf("    install:\n");
    printf("        -D, --no-depends  : Do not resolve dependencies and install\n");
    printf("                            only the specified packages.\n");
    printf("        -S, --sync        : Sync all package repositories before resolving the\n");
    printf("                            dependencies and installing the packages.\n");
    printf("        -r, --reinstall   : Install the package and all of its dependencies,\n");
    printf("            --force         even if it is already installed and no updates\n");
    printf("                            for any of said packages are available.\n");
    return 0;
}

int version() {
    printf("kawa %s\n", VERSION);
    return 0;
}
