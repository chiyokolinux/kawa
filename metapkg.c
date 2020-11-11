#include "metapkg.h"

int metapkg_gen_kawafile(char pkgname[]) {
    kawafile_dir_create(pkgname);
    
    FILE *fp;

    char path[strlen(INSTALLPREFIX)+32+strlen(pkgname)];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/kawafiles/");
    strcat(path, pkgname);
    strcat(path, "/Kawafile");
    
    // We just want to create the file without writing anything
    fp = fopen(path, "w");
    fputs("#!/bin/sh\n"
          "case \"$1\" in install) do_install; ;; remove) do_remove; ;; update) do_update; ;; *) "
          "echo \\\"Usage: $0 {install|remove|update}\\\"; exit 1; ;; esac ", fp);
    fclose(fp);

    int retval = 0;
    retval += chmod(path, S_IRWXU);
    
    printf(" Done\n");
    
    return retval;
}

int metapkg_install(char pkgname[]) {
    // meta packages only install their depends and remove them on uninstall,
    // but that's handled without Kawafiles,
    // so we only need to generate an empty Kawafile
    int retval = 0;
    printf("Installing %s...", pkgname);
    fflush(stdout);

    retval += metapkg_gen_kawafile(pkgname);
    kawafile_run(pkgname, "install");
    printf(".");
    fflush(stdout);
    printf(" Done\n");

    return retval;
}

int metapkg_remove(struct package *package) {
    printf("Removing %s...", package->name);
    fflush(stdout);
    for (int i = 0; i < package->depends.retc; i++) {
        kawafile_run(package->depends.retval[i], "remove");
    }
    printf(" Done\n");
    return 0;
}

int metapkg_update(char pkgname[]) {
    // meta packages may update depends, but I don't care
    // not because I don't want to, but because I don't need to
    // that stuff is handled in the dependency resolver
    printf("Updating %s... Done", pkgname);
    return 0;
}
