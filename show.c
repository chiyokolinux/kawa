#include "show.h" 

int show(char pkgname[]) {
    struct pkglist *database = get_all_packages();
    struct package *currpkg;
    for (int i = 0; i < database->pkg_count; i++) {
        currpkg = database->packages[i];
        if (!strcmp(currpkg->name, pkgname)) {
            printf("Information for package %s:\n-------------------------", pkgname);
            int dashes_total = strlen(pkgname);
            for (int dash = 0; dash < dashes_total; dash++)
                printf("-");
            printf("\n");
            printf("Name        : %s\n", pkgname);
            printf("Version     : %s\n", currpkg->version);
            printf("Maintainer  : %s\n", currpkg->maintainer);
            printf("Type        : %s\n", currpkg->type);
            printf("License     : %s\n", currpkg->license);
            printf("Description :\n    %s", currpkg->description);
            return 0;
        }
    }
    printf("Error: Package %s not found (try kawa sync)\n", pkgname);
    return 1;
}
