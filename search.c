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

#include "search.h"

int search(char searchterm[]) {
    struct pkglist *database = get_all_packages();
    struct pkglist *installed = get_installed_packages();
    putchar('\n');
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
            sprintf(name_colored, "%.*s\033%s%.*s\033[0m%s", firstpart_len, currpkg->name, HIGHLIGHTCOLOR, (int)strlen(searchterm), pos, pos+strlen(searchterm));
            printf(" %-3s | %-37s | %-14s | %s\n", package_installed, name_colored, currpkg->version, currpkg->type);
        }
    }
    return 0;
}
