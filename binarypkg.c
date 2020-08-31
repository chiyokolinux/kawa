#include "binarypkg.h"

// command to generate uninstall cmd:
// tar -tf package.tar.* | sed '/\/$/d' | sed -e 's/^/    rm \//'
// tar xf package.tar.* -C /
int binarypkg_gen_kawafile(char pkgname[], char filetype[]) {
    kawafile_dir_create(pkgname);

    char path[strlen(INSTALLPREFIX)+32+strlen(pkgname)];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/kawafiles/");
    strcat(path, pkgname);
    strcat(path, "/Kawafile");
    
    char cmdline[446+strlen(path)];
    strcpy(cmdline, "");
    // we'll make clean-installing the default behaviour to avoid file conflicts and bloathing the system with stale files (for example when a file name changes)
    sprintf(cmdline, "(echo \"#!/bin/sh\"; echo \"do_install() {\"; echo \"    tar xf package.tar.* -C /\"; echo \"}\"; echo \"do_remove() {\"; tar -tf package.tar.* | sed '/\\/$/d' | sed -e 's/^/    rm -f \\//'; echo \"}\"; echo \"do_update() {\"; echo \"    do_remove\"; echo \"    do_install\"; echo \"}\"; echo \"case \\\"\\$1\\\" in install) do_install; ;; remove) do_remove; ;; update) do_update; ;; *) echo \\\"Usage: $0 {install|remove|update}\\\"; exit 1; ;; esac\") > %s", path);
    
    int retval = system(cmdline);
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
    retval += binarypkg_gen_kawafile(pkgname, filetype);
    kawafile_run(pkgname, "install");
    printf(".");
    fflush(stdout);
    printf(" Done.\n");
    return retval;
}

int binarypkg_remove(char pkgname[]) {
    printf("Removing %s...", pkgname);
    fflush(stdout);
    kawafile_run(pkgname, "remove");
    printf(" Done.\n");
    return 0;
}

int binarypkg_update(char pkgname[]) {
    int retval = 0;
    printf("Updating %s.", pkgname);
    fflush(stdout);
    retval += binarypkg_gen_kawafile(pkgname);
    kawafile_run(pkgname, "update");
    printf(".");
    fflush(stdout);
    printf(" Done.\n");
    return retval;
}
