#include "binarypkg.h"

// command to generate uninstall cmd:
// tar -tf package.tar.* | sed '/\/$/d' | sed -e 's/^/    rm \//'
// tar xf package.tar.* -C /
int binarypkg_gen_kawafile(char pkgname[], char filetype[]) {
    char path[strlen(INSTALLPREFIX)+32+strlen(pkgname)];
    char dir[strlen(INSTALLPREFIX)+23+strlen(pkgname)];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/kawafiles/");
    strcat(path, pkgname);
    strcpy(dir, path);
    strcat(path, "/Kawafile");
    
    char cmdline[798+strlen(path)+strlen(dir)*2+strlen(filetype)*2];
    strcpy(cmdline, "");
    // we'll make clean-installing the default behaviour to avoid file conflicts and bloathing the system with stale files (for example when a file name changes)
    // do.install.sh and do.build.sh are not supported for binary pkgs (why should it be?), maybe in the future (although unlikely)
    sprintf(cmdline, "(echo \"#!/bin/sh\"; "
                     "echo \"cd %3$s\"; "
                     "echo \"perform_install() {\"; "
                     "echo \"    tar xf package.tar.%2$s -C /\"; "
                     "echo \"}\"; "
                     "echo \"do_install() {\"; "
                     "echo \"    [[ -f pre.install.sh ]] && ./pre.install.sh\"; "
                     "echo \"    perform_install\"; "
                     "echo \"    [[ -f post.install.sh ]] && ./post.install.sh\"; "
                     "echo \"}\"; "
                     "echo \"do_remove() {\"; "
                     "tar -tf %3$s/package.tar.%2$s | sed '/\\/$/d' | sed -e 's/^/    rm -f \\//'; "
                     "echo \"}\"; "
                     "echo \"do_update() {\"; "
                     "echo \"    [[ -f pre.update.sh ]] && ./pre.update.sh\"; "
                     "echo \"    do_remove\"; "
                     "echo \"    perform_install\"; "
                     "echo \"    [[ -f post.update.sh ]] && ./post.update.sh\"; "
                     "echo \"}\"; "
                     "echo \"case \\\"\\$1\\\" in install) do_install; ;; remove) do_remove; ;; update) do_update; ;; *) "
                     "echo \\\"Usage: $0 {install|remove|update}\\\"; exit 1; ;; esac\") "
                     "> %1$s", path, filetype, dir);
    
    int retval = system(cmdline);

    if (chmod(path, S_IRWXU) != 0)
        perror("chmod");

    printf(".");
    fflush(stdout);
    
    return retval;
}

int binarypkg_install(char pkgname[], char filetype[]) {
    // binary packages are basically untared on install and to uninstall,
    // we remove all files that were in the tar. so that's all our Kawafile does
    int retval = 0;
    printf("Installing %s.", pkgname);
    fflush(stdout);
    // generate kawafile
    retval += binarypkg_gen_kawafile(pkgname, filetype);
    // run kawafile install
    kawafile_run(pkgname, "install");
    printf(".");
    fflush(stdout);
    printf(" Done\n");
    return retval;
}

int binarypkg_remove(char pkgname[]) {
    printf("Removing %s...", pkgname);
    fflush(stdout);
    // run kawafile remove
    kawafile_run(pkgname, "remove");
    printf(" Done\n");
    return 0;
}

int binarypkg_update(char pkgname[], char filetype[]) {
    int retval = 0;
    printf("Updating %s.", pkgname);
    fflush(stdout);
    // update kawafile
    retval += binarypkg_gen_kawafile(pkgname, filetype);
    // run kawafile update
    kawafile_run(pkgname, "update");
    printf(".");
    fflush(stdout);
    printf(" Done\n");
    return retval;
}
