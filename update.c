#include "update.h"

struct pkg_update *pkg_has_update(char pkgname[], struct pkglist *database, struct pkglist *installed) {
    int remote_i = 0;
    int i = 0;
    while (strcmp(pkgname, installed->packages[i]->name))
        i++;
    struct package *currpkg_l = installed->packages[i];
    while (strcmp(currpkg_l->name, database->packages[remote_i]->name))
        remote_i++; // this should work because the package lists are sorted (hopefully)
    if (strcmp(currpkg_l->version, database->packages[remote_i]->version)) {
        struct pkg_update *pkgupdt;
        if (!(pkgupdt= malloc(sizeof(struct pkg_update)))) malloc_fail();
        *pkgupdt = (struct pkg_update) {
            .name = currpkg_l->name,
            .version_local = currpkg_l->version,
            .version_remote = database->packages[remote_i]->version
        };
        return pkgupdt;
    }
    return NULL;
}

int update() {
    sync_all();

    struct pkglist *database = get_all_packages();
    struct pkglist *installed = get_installed_packages();
    struct pkg_update *updatepkg[installed->pkg_count];
    struct package **packages;
    if (!(packages = malloc(sizeof(char) * database->pkg_count * 512))) malloc_fail();
    struct pkglist *nodelist;
    if (!(nodelist = malloc(sizeof(struct pkglist) + sizeof(char) * database->pkg_count * 512))) malloc_fail();
    nodelist->pkg_count = 0;
    nodelist->packages = packages;
    int remote_i = 0;
    int updatec = 0;

    for (int i = 0; i < installed->pkg_count; i++) {
        struct package *currpkg_l = installed->packages[i];
        while (strcmp(currpkg_l->name, database->packages[remote_i]->name))
            remote_i++; // this should work because the package lists are sorted (hopefully)
        struct package *currpkg_r = database->packages[remote_i];
        check_package_source(currpkg_r, remote_i, database, installed, 1);
        if (strcmp(currpkg_l->version, currpkg_r->version)) {
            struct pkg_update *pkgupdt;
            if (!(pkgupdt = malloc(sizeof(struct pkg_update)))) malloc_fail();
            *pkgupdt = (struct pkg_update) {
                .name = currpkg_l->name,
                .version_local = currpkg_l->version,
                .version_remote = currpkg_r->version
            };
            updatepkg[updatec++] = pkgupdt;
        }
    }
    
    if (!updatec) {
        printf("\nNo packages need to be updated.\n");
        return 0;
    }
    
    for (int i = 0; i < updatec; i++) {
        resolve_recursive(nodelist, NULL, updatepkg[i]->name, database, installed, 0, &updatec, 0, 1);
    }
    
    printf("\nThe following packages can be updated:\n  ");
    
    for (int i = 0; i < updatec; i++) {
        printf("%s (%s -> %s) ", updatepkg[i]->name, updatepkg[i]->version_local, updatepkg[i]->version_remote);
    }
    
    if (nodelist->pkg_count) {
        printf("\n\nThe following packages will be installed:\n ");
        for (int i = 0; i < nodelist->pkg_count; i++) {
            printf(" %s", nodelist->packages[i]->name);
        }
    }
    
    printf("\n\nDo you wish to install these updates now? [Y/n] ");
    fflush(stdout);
    char response = getchar();
    if (response == 'n' || response == 'N')
        return 0;
    else if (response == 'y' || response == 'Y' || response == '\n')
        return upgrade(updatepkg, updatec, database, installed, nodelist);
    else
        return 1;
}

int upgrade(struct pkg_update *updpkglst[], int updatec, struct pkglist *database, struct pkglist *installed, struct pkglist *nodelist) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    int retval = 0;
    
    // before updating, install all new dependencies
    for (int i = 0; i < nodelist->pkg_count; i++) {
        retval += install_no_deps(nodelist->packages[i], database, 0, 0);
    }
    
    // install updates and re-wire version pointers
    for (int i = 0; i < updatec; i++) {
        struct package *currpkg;
        int *ii = malloc(sizeof(int));
        currpkg = bsearch_pkg(updpkglst[i]->name, database, ii, 0);
        free(ii);

        retval += install_no_deps(currpkg, database, 0, 1);

        for (int i2 = 0; i2 < installed->pkg_count; i2++) {
            currpkg = installed->packages[i2];
            if (!strcmp(currpkg->name, updpkglst[i]->name)) {
                currpkg->version = updpkglst[i]->version_remote;
            }
        }
    }
    
    curl_global_cleanup();
    retval += write_installed_packages(installed, database);
    return retval;
}
