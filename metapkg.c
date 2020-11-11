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
    retval += fprintf(fp, "#!/bin/sh\n"
                          "cd %1$s\n"
                          "perform_install() {\n"
                          "    :\n"
                          "}\n"
                          "do_install() {\n"
                          "    [[ -f pre.install.sh ]] && ./pre.install.sh\n"
                          "    perform_install\n"
                          "    [[ -f post.install.sh ]] && ./post.install.sh\n"
                          "}\n"
                          "do_remove() {\n"
                          "    :\n"
                          "}\n"
                          "do_update() {\n"
                          "    [[ -f pre.update.sh ]] && ./pre.update.sh\n"
                          "    do_remove\n"
                          "    perform_install\n"
                          "    [[ -f post.update.sh ]] && ./post.update.sh\n"
                          "}\n"
                          "case \"$1\" in install) do_install; ;; remove) do_remove; ;; update) do_update; ;; *) "
                          "echo \"Usage: $0 {install|remove|update}\"; exit 1; ;; esac\n", dir);
    retval += fclose(fp);

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
