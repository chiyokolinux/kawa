#include "sourcepkg.h"

int sourcepkg_gen_kawafile(struct package *package, char filetype[]) {
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
    if (!strcmp(package->sepbuild, "yes")) {
        strcpy(enterbuilddir, "    mkdir build && cd build\n");
        strcpy(exitbuilddir, "    cd ../..\n");
    } else {
        strcpy(enterbuilddir, "");
        strcpy(exitbuilddir, "    cd ..\n");
    }

    fp = fopen(path, "w");
    retval += fprintf(fp, "#!/bin/sh\n"
                          "cd %1$s\n"
                          "prepare_files() {\n"
                          "    tar xf package.tar.%6$s\n"
                          "    cd $(tar tf package.tar.%6$s | head -n1)\n"
                          "%7$s"
                          "}\n"
                          "cleanup() {\n"
                          "%8$s"
                          "    rm -rf $(tar tf package.tar.%6$s | head -n1)\n"
                          "}\n"
                          "perform_install() {\n"
                          "    %3$s %4$s\n"
                          "    [[ -f do.build.sh ]] && ./do.build.sh || make -j%2$s\n"
                          "    [[ -f do.install.sh ]] && ./do.install.sh || make -j%2$s install\n"
                          "}\n"
                          "do_install() {\n"
                          "    prepare_files\n"
                          "    [[ -f pre.install.sh ]] && ./pre.install.sh\n"
                          "    perform_install\n"
                          "    [[ -f post.install.sh ]] && ./post.install.sh\n"
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
                          filetype, enterbuilddir, exitbuilddir);
    retval += fclose(fp);

    retval += chmod(path, S_IRWXU);

    printf(".");
    fflush(stdout);
    
    return retval;
}

int sourcepkg_install(struct package *package, char filetype[]) {
    int retval = 0;
    printf("Installing %s.", package->name);
    fflush(stdout);
    // generate kawafile
    retval += sourcepkg_gen_kawafile(package, filetype);
    // run kawafile install
    kawafile_run(package->name, "install");
    printf(".");
    fflush(stdout);
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

int sourcepkg_update(struct package *package, char filetype[]) {
    int retval = 0;
    printf("Updating %s.", package->name);
    fflush(stdout);
    // update kawafile
    retval += binarypkg_gen_kawafile(package->name, filetype);
    // run kawafile update
    kawafile_run(package->name, "update");
    printf(".");
    fflush(stdout);
    printf(" Done\n");
    return retval;
}
