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

#include "patchpkg.h"

int patchpkg_gen_kawafile(struct package *package) {
    FILE *fp;
    int retval = 0;

    char path[strlen(INSTALLPREFIX)+34+strlen(package->depends.retval[0])+strlen(package->name)];
    snprintf(path, strlen(INSTALLPREFIX)+34+strlen(package->depends.retval[0])+strlen(package->name), "%s/etc/kawa.d/kawafiles/%s/%s.patch.sh", INSTALLPREFIX, package->depends.retval[0], package->name);

    fp = fopen(path, "w");

    if (fp == NULL) {
        perror("fopen");
        exit(4);
    }

    retval += fprintf(fp, "#!/bin/sh\n"
                          "echo \"Applying %2$s ( %1$s )\"\n"
                      "patch %4$s %3$s\n", package->name, package->description, package->configurecmd, whitespace_join(package->configureopts));

    if (fchmod(fileno(fp), S_IRWXU) != 0)
        perror("fchmod");

    retval += fclose(fp);

    return retval;
}

int patchapkg_install(struct package *package) {
    int retval = 0;
    printf("Installing %s...", package->name);
    fflush(stdout);

    retval += patchpkg_gen_kawafile(package);
    printf(" Done\n");

    return retval;
}

int patchpkg_remove(struct package *package) {
    printf("Removing %s...", package->name);
    char path[strlen(INSTALLPREFIX)+34+strlen(package->depends.retval[0])+strlen(package->name)];
    snprintf(path, strlen(INSTALLPREFIX)+34+strlen(package->depends.retval[0])+strlen(package->name), "%s/etc/kawa.d/kawafiles/%s/%s.patch.sh", INSTALLPREFIX, package->depends.retval[0], package->name);
    int retval = remove(path);
    printf(" Done\n");
    return retval;
}

int patchpkg_update(struct package *package) {
    int retval = 0;
    printf("Updating %s... Done", package->name);
    fflush(stdout);

    retval += patchpkg_gen_kawafile(package);
    printf(" Done\n");

    return retval;
}
