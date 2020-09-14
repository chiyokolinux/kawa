#include "remove.h"

int pkg_remove(int pkgc, char *pkgnames[]) {
    struct pkglist *database = get_all_packages();
    struct pkglist *installed = get_installed_packages();
    
    // check if packages *can* be uninstalled
    for (int i = 2; i < pkgc; i++) {
        int package_installed = 0;
        for (int i2 = 0; i2 < installed->pkg_count; i2++) {
            if (!strcmp(pkgnames[i], installed->packages[i2]->name)) {
                package_installed = 1;
                break;
            }
        }
        if (!package_installed) {
            printf("Error: package %s is not installed", pkgnames[i]);
            return 1;
        }
    }
    
    printf("The following packages will be installed:\n ");
    for (int i = 0; i < pkgc; i++) {
        printf(" %s", pkgnames[i]);
    }
    
    printf("\n\n%d package(s) will be installed, %d package(s) will be removed and %d package(s) will be updated.\n", 0, pkgc, 0);
    printf("Do you wish to proceed? [Y/n] ");
    fflush(stdout);
    char response = getchar();
    if (response == 'n' || response == 'N')
        return 0;
    else if (response == 'y' || response == 'Y' || response == '\n') {
        int retval = 0;
        
        // after updating, install all new packages
        for (int i = 0; i < pkgc; i++) {
            retval += remove_single(pkgnames[i], database);
        }
        
        return retval;
    } else
        return 1;
    
    return 0;
}

int remove_single(char pkgname[], struct pkglist *database) {
    struct package *currpkg;
    for (int i = 0; i < database->pkg_count; i++) {
        currpkg = database->packages[i];
        if (!strcmp(currpkg->name, pkgname)) {
            // unregister package
            remove_db_entry(currpkg);
            
            // remove the package using the function provided by the package type class
            if (!strcmp(currpkg->type, "source"))
                return 0; // TODO: sourcepkg_install(name=pkgname)
            else if (!strcmp(currpkg->type, "patch"))
                return 0; // TODO: sourcepkg_install(patch=pkgname)
            else if (!strcmp(currpkg->type, "meta"))
                return metapkg_remove(currpkg);
            else if (!strcmp(currpkg->type, "binary"))
                return binarypkg_remove(pkgname);
            return 1;
        }
    }
    // this should not happen. if it does, oh well.
    printf("Error: Package %s not found (try kawa sync)\n", pkgname);
    return 1;
}
