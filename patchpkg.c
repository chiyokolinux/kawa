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
    // ensure that target dir is present
    kawafile_dir_create(package->depends.retval[0]);

    FILE *fp;
    int retval = 0;

    char path[strlen(INSTALLPREFIX)+34+strlen(package->depends.retval[0])+strlen(package->name)];
    snprintf(path, strlen(INSTALLPREFIX)+34+strlen(package->depends.retval[0])+strlen(package->name), "%s/etc/kawa.d/kawafiles/%s/%s.patch.sh", INSTALLPREFIX, package->depends.retval[0], package->name);

    puts(path);

    fp = fopen(path, "w");

    if (fp == NULL) {
        perror("fopen");
        exit(4);
    }

    // TODO: make patch be gzipped, unpack that into
    // package->configurecmd and then apply *that* patch file
    fprintf(fp, "#!/bin/sh\n"
                "echo \"Applying %2$s ( %1$s )\"\n"
                "patch %3$s -i $1/../%1$s/package.src.kawapkg\n",
                package->name, package->description,
                whitespace_join(package->configureopts)); // , package->configurecmd);

    if (fchmod(fileno(fp), S_IRWXU) != 0)
        perror("fchmod");

    retval += fclose(fp);

    return retval;
}

int patchpkg_install(struct package *package) {
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
