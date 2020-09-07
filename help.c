#include "help.h"

int help() {
    printf("kawa - help\n");
    printf("-----------\n\n");
    printf("basic commands:\n");
    printf("    kawa install [package(s)]\n");
    printf("    kawa remove [package(s)]\n");
    printf("    kawa help\n");
    printf("    kawa makepackages\n");
    printf("    kawa update\n");
    printf("    kawa search [term]\n");
    printf("    kawa show [package]\n");
    printf("    kawa sync\n");
    printf("    kawa version\n");
    printf("\ncommand options:\n");
    printf("    install:\n");
    printf("        -D, --no-depends  : Do not resolve dependencies and install\n");
    printf("                            only the specified packages.\n");
    return 0;
}

int version() {
    printf("kawa %s\n", VERSION);
    return 0;
}
