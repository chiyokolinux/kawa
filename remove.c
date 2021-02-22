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
            fprintf(stderr, "Error: package %s is not installed\n", pkgnames[i]);
            return 1;
        }
    }
    
    printf("\nThe following packages will be removed:\n ");
    for (int i = 2; i < pkgc; i++) {
        printf(" %s", pkgnames[i]);
    }
    
    printf("\n\n%d package(s) will be installed, %d package(s) will be removed and %d package(s) will be updated.\n", 0, pkgc-2, 0);
    printf("Do you wish to proceed? [Y/n] ");
    fflush(stdout);
    char response = getchar();
    if (response == 'n' || response == 'N')
        return 0;
    else if (response == 'y' || response == 'Y' || response == '\n') {
        int retval = 0;
        
        // remove requested packages
        for (int i = 2; i < pkgc; i++) {
            retval += remove_single(pkgnames[i], installed, database);
        }
        
        retval += write_installed_packages(installed, database);
        
        return retval;
    } else
        return 1;
    
    return 0;
}

int remove_single(char pkgname[], struct pkglist *installed, struct pkglist *database) {
    struct package *currpkg;
    int *i;
    if (!(i = malloc(sizeof(int)))) malloc_fail();
    currpkg = bsearch_pkg(pkgname, installed, i, 0);
    free(i);

    struct package *currpkg_db;
    if (!(i = malloc(sizeof(int)))) malloc_fail();
    currpkg_db = bsearch_pkg(pkgname, database, i, 0);
    free(i);

    // unregister package
    remove_db_entry(currpkg, installed);
    
    int retval = 0;
    
    // remove the package using the function provided by the package type class
    // use database version because of the way metapkg handle dependency removal
    if (!strcmp(currpkg->type, "source"))
        retval = sourcepkg_remove(currpkg_db);
    else if (!strcmp(currpkg->type, "patch"))
        retval = 0; // TODO: sourcepkg_install(patch=pkgname)
    else if (!strcmp(currpkg->type, "meta"))
        retval = metapkg_remove(currpkg_db);
    else if (!strcmp(currpkg->type, "binary"))
        retval = binarypkg_remove(pkgname);

    kawafile_dir_remove(currpkg);
    return retval;
}

void remove_db_entry(struct package *package, struct pkglist *installed) {
    // we're just removing (and free-ing, hopefully this won't cause problems later (spoiler alert: it did))
    // the pointer to the package in *installed, and at the end we'll re-write Installed.packages.db
    struct package *currpkg;
    for (int i = 0; i < installed->pkg_count; i++) {
        currpkg = installed->packages[i];
        // we're comparing pointers, so this (should) work(s).
        if (currpkg == package) {
            installed->packages[i] = NULL;
            // free(package); caused problems
        }
    }
}
