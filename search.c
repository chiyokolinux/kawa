#include "search.h"

int search(char searchterm[]) {
    struct pkglist *database = get_all_packages();
    // struct pkglist *installed = get_installed_packages();
    // int local_i = 0;
    printf("Inst | Name                       | Version        | Type\n");
    printf("-----|----------------------------|----------------|-------\n");
    for (int i = 0; i < database->pkg_count; i++) {
        struct package *currpkg = database->packages[i];
        if (strstr(currpkg->name, searchterm) != NULL) {
            printf("%4s | %-26s | %-14s | %s\n", "???", currpkg->name, currpkg->version, currpkg->type);
        }
    }
    return 0;
}
