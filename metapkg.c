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
    fputs("#!/bin/sh\n", fp);
    fclose(fp);
    
    return 0;
}

int metapkg_install(char pkgname[]) {
    // meta packages only install their depends and remove them on uninstall,
    // but that's handled without Kawafiles,
    // so we only need to generate an empty Kawafile
    return metapkg_gen_kawafile(pkgname);
}

int metapkg_remove(struct package *package) {
    for (int i = 0; i < package->depends.retc; i++) {
        // uninstall dat boi
    }
    return 0;
}
