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
    char exitbuilddir[15];
    char installdestdir[strlen(dir)+9];
    char filelistfile[17];
    if (!strcmp(package->sepbuild, "yes")) {
        strcpy(enterbuilddir, "    mkdir build && cd build\n");
        strcpy(exitbuilddir, "    cd ../..\n");
        strcpy(filelistfile, "../../files.list");
    } else {
        strcpy(enterbuilddir, "");
        strcpy(exitbuilddir, "    cd ..\n");
        strcpy(filelistfile, "../files.list");
    }
    strcpy(installdestdir, dir);
    strcat(installdestdir, "/install");

    fp = fopen(path, "w");

    if (fp == NULL) {
        perror("fopen");
        exit(4);
    }

    retval += fprintf(fp, "#!/bin/sh\n"
                          "cd %1$s\n"
                          "prepare_files() {\n"
                          "    tar xf package.src.kawapkg\n"
                          "    cd $(tar tf package.src.kawapkg | head -n1)\n"
                          "%6$s"
                          "    rm -rf %8$s\n"
                          "    mkdir %8$s\n"
                          "}\n"
                          "cleanup() {\n"
                          "%7$s"
                          "    rm -rf $(tar tf package.src.kawapkg | head -n1)\n"
                          "}\n"
                          "file_uninstall() {\n"
                          "    rm -f $(xargs -a %9$s)\n"
                          "}\n"
                          "perform_install() {\n"
                          "    %3$s %4$s\n"
                          "    [ -f do.build.sh ] && ./do.build.sh || make -j%2$s\n"
                          "    [ -f do.install.sh ] && ./do.install.sh || make -j%2$s install DESTDIR=%8$s\n"
                          "    pushd %8$s\n"
                          "    find . -type f -print | cut -c 2- > %9$s\n"
                          "    cp -RT . %10$s/\n"
                          "    popd\n"
                          "    rm -rf %8$s\n"
                          "}\n"
                          "do_install() {\n"
                          "    prepare_files\n"
                          "    [ -f pre.install.sh ] && ./pre.install.sh\n"
                          "    [ -f patch.apply.sh ] && ./patch.apply.sh\n"
                          "    perform_install\n"
                          "    [ -f post.install.sh ] && ./post.install.sh\n"
                          "    cleanup\n"
                          "}\n"
                          "do_remove() {\n"
                          "    prepare_files\n"
                          "    %5$s\n"
                          "    cleanup\n"
                          "}\n"
                          "do_update() {\n"
                          "    prepare_files\n"
                          "    [[ -f pre.update.sh ]] && ./pre.update.sh\n"
                          "    perform_install\n"
                          "    [[ -f post.update.sh ]] && ./post.update.sh\n"
                          "    cleanup\n"
                          "}\n"
                          "case \"$1\" in install) do_install; ;; remove) do_remove; ;; update) do_update; ;; *) "
                          "echo \"Usage: $0 {install|remove|update}\"; exit 1; ;; esac\n",
                          dir, THREADNUM, package->configurecmd, whitespace_join(package->configureopts), package->uninstallcmd,
                          enterbuilddir, exitbuilddir, installdestdir, filelistfile, INSTALLPREFIX);

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
    kawafile_run(package->name, "install");
    printf(" Done\n");
    return retval;
}

int sourcepkg_remove(struct package *package) {
    printf("Removing %s...", package->name);
    fflush(stdout);
    // run kawafile remove
    kawafile_run(package->name, "remove");
    printf(" Done\n");
    return 0;
}

int sourcepkg_update(struct package *package) {
    int retval = 0;
    printf("Updating %s.", package->name);
    fflush(stdout);
    // update kawafile
    retval += binarypkg_gen_kawafile(package->name);
    // run kawafile update
    kawafile_run(package->name, "update");
    printf(".");
    fflush(stdout);
    printf(" Done\n");
    return retval;
}
