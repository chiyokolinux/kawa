#include "search.h"

int search(char searchterm[]) {
    struct pkglist *database = get_all_packages();
    struct pkglist *installed = get_installed_packages();
    printf("Inst | Name                       | Version        | Type\n");
    printf("-----|----------------------------|----------------|-------\n");
    for (int i = 0; i < database->pkg_count; i++) {
        struct package *currpkg = database->packages[i];
        char *pos = strstr(currpkg->name, searchterm);
        if (pos != NULL) {
            char package_installed[4] = "no";
            for (int i2 = 0; i2 < installed->pkg_count; i2++) {
                if (!strcmp(currpkg->name, installed->packages[i2]->name)) {
                    strcpy(package_installed, "yes");
                    break;
                }
            }
            char name_colored[strlen(currpkg->name) + strlen(HIGHLIGHTCOLOR) + 9];
            int firstpart_len = strlen(currpkg->name) - strlen(pos);
            // int 
            sprintf(name_colored, "%.*s\033%s%.*s\033[0m%s", firstpart_len, currpkg->name, HIGHLIGHTCOLOR, strlen(searchterm), pos, pos+strlen(searchterm));
            printf("%4s | %-37s | %-14s | %s\n", package_installed, name_colored, currpkg->version, currpkg->type);
        }
    }
    return 0;
}
