#include "metapkg.h"

int metapkg_gen_kawafile(char pkgname[], struct package *pkgobj) {
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

int metapkg_install(char pkgname[], struct package *pkgobj) {
    // meta packages only install their depends and remove them on uninstall,
    // but that's handled without Kawafiles,
    // so we only need to generate an empty Kawafile
    return metapkg_gen_kawafile(pkgname, pkgobj);
}

int metapkg_remove(char pkgname[]) {
    // TODO: change that to for i in depends: uninstall(package)
    return 0;
}
