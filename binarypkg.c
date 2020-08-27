#include "binarypkg.h"

// command to generate uninstall cmd:
// tar -tf packages/wallpaper-chungus/archives/wallpaper-chungus-1.0.tar.gz | sed '/\/$/d' | sed -e 's/^/rm \//'
int binarypkg_gen_kawafile(char pkgname[], struct package *pkgobj) {
    kawafile_dir_create(pkgname);
    
    FILE *fp;

    char path[strlen(INSTALLPREFIX)+32+strlen(pkgname)];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/kawafiles/");
    strcat(path, pkgname);
    strcat(path, "/Kawafile");
    
    printf("%s", pkgobj->name);
    
    system("for d in packages/*/; do ${d}/mkpackage.sh >> Kawamake.log; done;");
    printf(".");
    fflush(stdout);
    
    // We just want to create the file without writing anything
    fp = fopen(path, "w");
    fputs("#!/bin/sh\n", fp);
    fclose(fp);
    
    return 0;
}

int binarypkg_install(char pkgname[], struct package *pkgobj) {
    // binary packages are basically untared on install and to uninstall,
    // we remove all files that were in the tar. so that's all our Kawafile does
    int retval = 0;
    printf("Installing %s.", pkgname);
    fflush(stdout);
    retval += binarypkg_gen_kawafile(pkgname, pkgobj);
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
