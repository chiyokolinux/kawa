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

#include "binarypkg.h"

// command to generate uninstall cmd:
// tar -tf package.src.kawapkg | sed '/\/$/d' | sed -e 's/^/    rm \//'
// tar xf package.src.kawapkg -C /
int binarypkg_gen_kawafile(struct package *package) {
    char path[strlen(INSTALLPREFIX)+32+strlen(package->name)];
    char dir[strlen(INSTALLPREFIX)+23+strlen(package->name)];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/kawafiles/");
    strcat(path, package->name);
    strcpy(dir, path);
    strcat(path, "/Kawafile");

    char rmcmd[255];
    if (package->uninstallcmd[0] == '\0') {
        strcpy(rmcmd, "file_uninstall");
    } else {
        strncpy(rmcmd, package->uninstallcmd, 254);
        rmcmd[254] = '\0';
    }
    
    char cmdline[972+strlen(path)+strlen(dir)*2];
    strcpy(cmdline, "");
    // we'll make clean-installing the default behaviour to avoid file conflicts and bloathing the system with stale files (for example when a file name changes)
    // do.install.sh and do.build.sh are not supported for binary pkgs (why should it be?), maybe in the future (although unlikely)
    sprintf(cmdline, "(echo \"#!/bin/sh\"; "
                     "echo \"cd %2$s\"; "
                     "echo \"export DESTDIR=%3$s\"; "
                     "echo \"file_uninstall() {\"; "
                     "tar tf %2$s/package.src.kawapkg | sed '/\\/$/d' | sed -e 's/^/    rm -f \\${DESTDIR}\\//'; "
                     "echo \"}\"; "
                     "echo \"perform_install() {\"; "
                     "echo \"    tar xf package.src.kawapkg -C %3$s/\"; "
                     "echo \"}\"; "
                     "echo \"do_install() {\"; "
                     "echo \"    [ -f pre.install.sh ] && { ./pre.install.sh || exit 1; }\"; "
                     "echo \"    perform_install\"; "
                     "echo \"    [ -f post.install.sh ] && { ./post.install.sh || exit 1; }\"; "
                     "echo \"    return 0\"; "
                     "echo \"}\"; "
                     "echo \"do_remove() {\"; "
                     "echo \"    %4$s\"; "
                     "echo \"    return 0\"; "
                     "echo \"}\"; "
                     "echo \"do_update() {\"; "
                     "echo \"    [ -f pre.update.sh ] && { ./pre.update.sh || exit 1; }\"; "
                     "echo \"    do_remove\"; "
                     "echo \"    perform_install\"; "
                     "echo \"    [ -f post.update.sh ] && { ./post.update.sh || exit 1; }\"; "
                     "echo \"    return 0\"; "
                     "echo \"}\"; "
                     "echo \"case \\\"\\$1\\\" in install) do_install; ;; remove) do_remove; ;; update) do_update; ;; *) "
                     "echo \\\"Usage: $0 {install|remove|update}\\\"; exit 1; ;; esac\") "
                     "> %1$s", path, dir, INSTALLPREFIX, rmcmd);
    
    int retval = system(cmdline);

    if (chmod(path, S_IRWXU) != 0)
        perror("chmod");

    printf(".");
    fflush(stdout);
    
    return retval;
}

int binarypkg_install(struct package *package) {
    // binary packages are basically untared on install and to uninstall,
    // we remove all files that were in the tar. so that's all our Kawafile does
    int retval = 0;
    printf("Installing %s.", package->name);
    fflush(stdout);
    // generate kawafile
    retval += binarypkg_gen_kawafile(package);
    // run kawafile install
    retval += kawafile_run(package->name, "install");
    printf(".");
    fflush(stdout);
    printf(" Done\n");
    return retval;
}

int binarypkg_remove(char pkgname[]) {
    printf("Removing %s...", pkgname);
    fflush(stdout);
    // run kawafile remove
    int retval = kawafile_run(pkgname, "remove");
    printf(" Done\n");
    return retval;
}

int binarypkg_update(struct package *package) {
    int retval = 0;
    printf("Updating %s.", package->name);
    fflush(stdout);
    // update kawafile
    retval += binarypkg_gen_kawafile(package);
    // run kawafile update
    retval += kawafile_run(package->name, "update");
    printf(".");
    fflush(stdout);
    printf(" Done\n");
    return retval;
}
