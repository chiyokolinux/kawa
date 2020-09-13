#include "remove.h"

int pkg_remove(int pkgc, char *pkgnames[]) {
    for (int i = 2; i < pkgc; i++) {
        int package_installed = 0;
        for (int i2 = 0; i2 < installed->pkg_count; i2++) {
            if (!strcmp(pkgname, installed->packages[i2]->name)) {
                package_installed = 1;
                break;
            }
        }
        if (package_installed)
            kawafile_run(pkgnames[i], "remove");
        else {
            printf("Error: package %s is not installed", pkgnames[i]);
            return 1;
        }
    }
    return 0;
}
