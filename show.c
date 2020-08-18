#include "show.h" 

int show(char pkgname[]) {
    struct pkglist *database = get_all_packages();
    struct package *currpkg;
    for (int i = 0; i < database->pkg_count; i++) {
        currpkg = database->packages[i];
        if (!strcmp(currpkg->name, pkgname)) {
            printf("package found\n");
            return 0;
        }
    }
    printf("package not found\n");
    return 1;
}
