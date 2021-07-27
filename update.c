/**
 * this file is part of kawa
 * Copyright (c) 2020-2021 Emily <elishikawa@jagudev.net>
 *
 * kawa is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * kawa is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kawa.  If not, see <https://www.gnu.org/licenses/>.
**/

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
    
    // dependency types
    unsigned int *deptypes;
    if (!(deptypes = malloc(sizeof(unsigned int) * 5))) malloc_fail();
    int current_deptype = 0, current_bytepos = 0, current_char = 0;

    while (DEPTYPES[current_char++] != '\0') {
        if (DEPTYPES[current_char] == ' ') {
            current_deptype++;
            current_bytepos = 0;
        } else {
            deptypes[current_deptype] |= DEPTYPES[current_char] << current_bytepos;
            current_bytepos += 8;
        }
    }
    deptypes[++current_deptype] = 0;

    for (int i = 0; i < updatec; i++) {
        // TODO: pass deptype array from pkg deptype
        resolve_recursive(nodelist, NULL, updatepkg[i]->name, database, installed, 0, &updatec, 0, 1, deptypes);
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
    int retval = 0, failc = 0;

    // first, download all package archives and register all packages
    for (int i = 0; i < updatec; i++) {
        struct package *currpkg;
        int *ii;
        if (!(ii = malloc(sizeof(int)))) malloc_fail();
        currpkg = bsearch_pkg(updpkglst[i]->name, database, ii, 0);
        free(ii);

        retval = download_package(currpkg, database, 1);

        if (retval) {
            if (failc < MAXDOWNLOADRETRY) {
                fprintf(stderr, "Retrying download of package %s due to above error.\n", updpkglst[i]->name);
                failc++;
                i--;
            } else {
                fprintf(stderr, "Download of package %s failed. Aborting.\n", updpkglst[i]->name);
                curl_global_cleanup();
                return retval;
            }
        } else {
            failc = 0;
        }
    }
    for (int i = 0; i < nodelist->pkg_count; i++) {
        retval = download_package(nodelist->packages[i], database, 0);

        if (retval) {
            if (failc < MAXDOWNLOADRETRY) {
                fprintf(stderr, "Retrying download of package %s due to above error.\n", nodelist->packages[i]->name);
                failc++;
                i--;
            } else {
                fprintf(stderr, "Download of package %s failed. Aborting.\n", nodelist->packages[i]->name);
                curl_global_cleanup();
                return retval;
            }
        } else {
            failc = 0;
        }
    }

    // go offline, for long upgrades / upgrades where network software is changed
    curl_global_cleanup();
    
    // before updating, install all new dependencies
    for (int i = 0; i < nodelist->pkg_count; i++) {
        nodelist->packages[i]->depends = split_space(strdup(DEPTYPES));
        retval += install_no_deps(nodelist->packages[i], database, installed, 0, 0);
    }
    
    // install updates and re-wire version pointers
    for (int i = 0; i < updatec; i++) {
        struct package *currpkg;
        int *ii;
        if (!(ii = malloc(sizeof(int)))) malloc_fail();
        currpkg = bsearch_pkg(updpkglst[i]->name, database, ii, 0);
        free(ii);

        retval += install_no_deps(currpkg, database, installed, 0, 1);

        for (int i2 = 0; i2 < installed->pkg_count; i2++) {
            currpkg = installed->packages[i2];
            if (!strcmp(currpkg->name, updpkglst[i]->name)) {
                currpkg->version = updpkglst[i]->version_remote;
            }
        }
    }

    retval += write_installed_packages(installed, database);
    return retval;
}
