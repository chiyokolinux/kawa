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

#include "metapkg.h"

int metapkg_gen_kawafile(char pkgname[]) {
    kawafile_dir_create(pkgname);

    FILE *fp;
    int retval = 0;

    char path[strlen(INSTALLPREFIX)+32+strlen(pkgname)];
    char dir[strlen(INSTALLPREFIX)+23+strlen(pkgname)];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/kawafiles/");
    strcat(path, pkgname);
    strcpy(dir, path);
    strcat(path, "/Kawafile");

    // We just want to create a basic kawafile that just executes our scripts
    // neither do.build.sh nor do.install.sh are executed in metapkgs, as metapkgs should
    // only install dependencies and not do anything by themselves.
    // if you need you metapkg to install something, for example a configuration file, add
    // a (binary) package as dependency with all the files you need to add
    fp = fopen(path, "w");

    if (fp == NULL) {
        perror("fopen");
        exit(4);
    }

    fprintf(fp, "#!/bin/sh\n"
                "cd %1$s\n"
                "perform_install() {\n"
                "    :\n"
                "}\n"
                "do_install() {\n"
                "    [[ -f pre.install.sh ]] && { ./pre.install.sh || exit 1; }\n"
                "    perform_install\n"
                "    [[ -f post.install.sh ]] && { ./post.install.sh || exit 1; }\n"
                "    return 0\n"
                "}\n"
                "do_remove() {\n"
                "    :\n"
                "}\n"
                "do_update() {\n"
                "    [[ -f pre.update.sh ]] && { ./pre.update.sh || exit 1; }\n"
                "    do_remove\n"
                "    perform_install\n"
                "    [[ -f post.update.sh ]] && { ./post.update.sh || exit 1; }\n"
                "    return 0\n"
                "}\n"
                "case \"$1\" in install) do_install; ;; remove) do_remove; ;; update) do_update; ;; *) "
                "echo \"Usage: $0 {install|remove|update}\"; exit 1; ;; esac\n", dir);

    if (fchmod(fileno(fp), S_IRWXU) != 0)
        perror("fchmod");

    retval += fclose(fp);

    return retval;
}

int metapkg_install(char pkgname[]) {
    // meta packages only install their depends and remove them on uninstall,
    // but that's handled without Kawafiles,
    // so we only need to generate an empty Kawafile
    int retval = 0;
    printf("Installing %s..", pkgname);
    fflush(stdout);

    retval += metapkg_gen_kawafile(pkgname);
    printf(".");
    fflush(stdout);
    retval += kawafile_run(pkgname, "install");
    printf(" Done\n");

    return retval;
}

int metapkg_remove(struct package *package) {
    // TODO: find a way to display removed deps in user dialog
    int retval = 0;
    printf("Removing %s...", package->name);
    fflush(stdout);
    for (int i = 0; i < package->depends.retc; i++) {
        retval += kawafile_run(package->depends.retval[i], "remove");
    }
    printf(" Done\n");
    return retval;
}

int metapkg_update(char pkgname[]) {
    // meta packages may update depends, but I don't care
    // not because I don't want to, but because I don't need to
    // that stuff is handled in the dependency resolver
    printf("Updating %s... Done", pkgname);
    return 0;
}
