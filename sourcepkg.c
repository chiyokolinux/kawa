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

#include "sourcepkg.h"

int sourcepkg_gen_kawafile(struct package *package) {
    kawafile_dir_create(package->name);
    
    FILE *fp;
    int retval = 0;

    char path[strlen(INSTALLPREFIX)+32+strlen(package->name)];
    char dir[strlen(INSTALLPREFIX)+23+strlen(package->name)];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/kawafiles/");
    strcat(path, package->name);
    strcpy(dir, path);
    strcat(path, "/Kawafile");
    
    char enterbuilddir[30];
    char exitbuilddir[8];
    char installdestdir[strlen(dir)+9];
    char rmcmd[255];
    if (!strcmp(package->sepbuild, "yes")) {
        strcpy(enterbuilddir, "    mkdir build && cd build\n");
        strcpy(exitbuilddir, "../..");
    } else {
        strcpy(enterbuilddir, "");
        strcpy(exitbuilddir, "..");
    }
    strcpy(installdestdir, dir);
    strcat(installdestdir, "/install");
    if (package->uninstallcmd[0] == '\0') {
        strcpy(rmcmd, "file_uninstall");
    } else {
        strncpy(rmcmd, package->uninstallcmd, 254);
        rmcmd[254] = '\0';
    }

    fp = fopen(path, "w");

    if (fp == NULL) {
        perror("fopen");
        exit(4);
    }

    fprintf(fp, "#!/bin/sh\n"
                "cd %1$s\n"
                "export DESTDIR=%8$s\n"
                "export CC=cc\n"
                "die() {\n"
                "    popd; popd\n"
                "    cleanup\n"
                "    rm -rf %8$s\n"
                "    exit 1\n"
                "}\n"
                "prepare_files() {\n"
                "    tar xf package.src.kawapkg || die\n"
                "    cd $(tar tf package.src.kawapkg | head -n1)\n"
                "%6$s"
                "    rm -rf %8$s\n"
                "    mkdir %8$s\n"
                "}\n"
                "cleanup() {\n"
                "    cd %7$s\n"
                "    rm -rf $(tar tf package.src.kawapkg | head -n1)\n"
                "}\n"
                "file_uninstall() {\n"
                "    rm -f $(xargs -a %7$s/files.list)\n"
                "}\n"
                "perform_install() {\n"
                "    %3$s %4$s\n"
                "    [ -f %7$s/do.build.sh ] && { %7$s/do.build.sh || die; } || make -j%2$s\n"
                "    [ -f %7$s/do.install.sh ] && { %7$s/do.install.sh || die; } || make -j%2$s install\n"
                "    pushd %8$s\n"
                "    find . -type f -print | cut -c 2- > %7$s/files.list\n"
                "    cp -RT . %9$s/\n"
                "    popd\n"
                "    rm -rf %8$s\n"
                "}\n"
                "do_install() {\n"
                "    prepare_files\n"
                "    [ -f %7$s/pre.install.sh ] && { %7$s/pre.install.sh || die; }\n"
                "    find . -name \"*.patch.sh\" -exec {} \\;\n"
                "    perform_install\n"
                "    [ -f %7$s/post.install.sh ] && { %7$s/post.install.sh || die; }\n"
                "    cleanup\n"
                "    return 0\n"
                "}\n"
                "do_remove() {\n"
                "    prepare_files\n"
                "    %5$s\n"
                "    cleanup\n"
                "    return 0\n"
                "}\n"
                "do_update() {\n"
                "    prepare_files\n"
                "    [[ -f %7$s/pre.update.sh ]] && { %7$s/pre.update.sh || die; }\n"
                "    perform_install\n"
                "    [[ -f %7$s/post.update.sh ]] && { %7$s/post.update.sh || die; }\n"
                "    cleanup\n"
                "    return 0\n"
                "}\n"
                "case \"$1\" in install) do_install; ;; remove) do_remove; ;; update) do_update; ;; *) "
                "echo \"Usage: $0 {install|remove|update}\"; exit 1; ;; esac\n",
                dir, THREADNUM, package->configurecmd, whitespace_join(package->configureopts), rmcmd,
                enterbuilddir, exitbuilddir, installdestdir, INSTALLPREFIX);

    if (fchmod(fileno(fp), S_IRWXU) != 0)
        perror("fchmod");

    retval += fclose(fp);

    printf(".");
    fflush(stdout);
    
    return retval;
}

int sourcepkg_install(struct package *package) {
    int retval = 0;
    printf("Installing %s.", package->name);
    fflush(stdout);
    // generate kawafile
    retval += sourcepkg_gen_kawafile(package);
    printf(".");
    fflush(stdout);
    // run kawafile install
    retval += kawafile_run(package->name, "install");
    printf(" Done\n");
    return retval;
}

int sourcepkg_remove(struct package *package) {
    printf("Removing %s...", package->name);
    fflush(stdout);
    // run kawafile remove
    int retval = kawafile_run(package->name, "remove");
    printf(" Done\n");
    return retval;
}

int sourcepkg_update(struct package *package) {
    int retval = 0;
    printf("Updating %s.", package->name);
    fflush(stdout);
    // update kawafile
    retval += sourcepkg_gen_kawafile(package);
    // run kawafile update
    retval += kawafile_run(package->name, "update");
    printf(".");
    fflush(stdout);
    printf(" Done\n");
    return retval;
}
