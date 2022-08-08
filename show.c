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

#include "show.h" 

int show(char pkgname[]) {
    struct pkglist *database = get_all_packages();
    struct pkglist *installed = get_installed_packages();
    struct package *currpkg;

    int *i;
    if (!(i = malloc(sizeof(int)))) malloc_fail();
    currpkg = bsearch_pkg(pkgname, database, i, 0);

    char package_installed[16] = "no";
    for (int i2 = 0; i2 < installed->pkg_count; i2++) {
        if (!strcmp(pkgname, installed->packages[i2]->name)) {
            if (!strcmp("auto", installed->packages[i2]->description)) {
                strcpy(package_installed, "yes (automatic)");
            } else {
                strcpy(package_installed, "yes (manual)");
            }
            break;
        }
    }
    check_package_source(currpkg, *i, database, installed, (package_installed[0] == 'y'));
    printf("\nInformation for package %s:\n-------------------------", pkgname);
    int dashes_total = strlen(pkgname);
    for (int dash = 0; dash < dashes_total; dash++)
        putchar('-');
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
