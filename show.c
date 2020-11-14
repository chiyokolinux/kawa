#include "show.h" 

int show(char pkgname[]) {
    struct pkglist *database = get_all_packages();
    struct pkglist *installed = get_installed_packages();
    struct package *currpkg;
    for (int i = 0; i < database->pkg_count; i++) {
        currpkg = database->packages[i];
        if (!strcmp(currpkg->name, pkgname)) {
            char package_installed[4] = "no";
            for (int i2 = 0; i2 < installed->pkg_count; i2++) {
                if (!strcmp(pkgname, installed->packages[i2]->name)) {
                    strcpy(package_installed, "yes");
                    break;
                }
            }
            check_package_source(currpkg, i, database, installed, (package_installed[0] == 'y'));
            printf("\nInformation for package %s:\n-------------------------", pkgname);
            int dashes_total = strlen(pkgname);
            for (int dash = 0; dash < dashes_total; dash++)
                printf("-");
            printf("\n");
            printf("Name        : %s\n", pkgname);
            printf("Version     : %s\n", currpkg->version);
            printf("Installed   : %s\n", package_installed);
            printf("Maintainer  : %s\n", currpkg->maintainer);
            printf("Repository  : %s\n", database->repos->repos[*currpkg->repoindex]->reponame);
            printf("Type        : %s\n", currpkg->type);
            printf("License     : %s\n", currpkg->license);
            printf("Description :\n    %s\n", currpkg->description);
            return 0;
        }
    }
    printf("Error: Package %s not found (try kawa sync)\n", pkgname);
    return 1;
}
