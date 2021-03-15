#include "metapkg.h"

int patchpkg_gen_kawafile(struct package *package) {
    FILE *fp;
    int retval = 0;

    char path[strlen(INSTALLPREFIX)+32+strlen(package->depends.retval[0])];
    char dir[strlen(INSTALLPREFIX)+23+strlen(package->depends.retval[0])];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/kawafiles/");
    strcat(path, package->depends.retval[0]);
    strcpy(dir, path);
    strcat(path, "/Kawafile");

    fp = fopen(path, "w");

    if (fp == NULL) {
        perror("fopen");
        exit(4);
    }

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

    if (fchmod(fileno(fp), S_IRWXU) != 0)
        perror("fchmod");

    retval += fclose(fp);

    return retval;
}

int patchapkg_install(struct package *package) {
    int retval = 0;
    printf("Installing %s...", package->name);
    fflush(stdout);

    retval += patchpkg_gen_kawafile(package);
    printf(" Done\n");

    return retval;
}

int patchpkg_remove(struct package *package) {
    printf("Removing %s...", package->name);
    fflush(stdout);
    for (int i = 0; i < package->depends.retc; i++) {
        kawafile_run(package->depends.retval[i], "remove");
    }
    printf(" Done\n");
    return 0;
}

int patchpkg_update(struct package *package) {
    printf("Updating %s... Done", package->name);
    return 0;
}
