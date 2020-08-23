#include "install.h"

int install(int pkgc, char *pkgnames[]) {
    return 0;
}

int install_no_deps(char pkgname[]) {
    struct pkglist *database = get_all_packages();
    struct package *currpkg;
    for (int i = 0; i < database->pkg_count; i++) {
        currpkg = database->packages[i];
        if (!strcmp(currpkg->name, pkgname)) {
            if (!strcmp(currpkg->type, "source"))
                return 0; // TODO: sourcepkg_install(name=pkgname)
            else if (!strcmp(currpkg->type, "patch"))
                return 0; // TODO: sourcepkg_install(patch=pkgname)
            else if (!strcmp(currpkg->type, "meta"))
                return 0; // TODO: metapkg_install(name=pkgname)
            else if (!strcmp(currpkg->type, "binary"))
                return 0; // binarypkg_install(name=pkgname)
            return 1;
        }
    }
    printf("Error: Package %s not found (try kawa sync)\n", pkgname);
    return 1;
}
