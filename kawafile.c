#include "kawafile.h"

void spawnwait(char *const argv[]) { // copied from ichirou
    pid_t chpid = fork();
    switch (chpid) {
        case 0:
            sigprocmask(SIG_UNBLOCK, &set, NULL);
            setsid();
            execvp(argv[0], argv);
            perror("execvp");
            _exit(1);
        case -1:
            perror("fork");
        default:
            waitpid(chpid, NULL, WUNTRACED);
    }
}

void kawafile_run(char pkgname[], char operation[]) {
    // argv = INSTALLPREFIX + /etc/kawa.d/kawafiles/${pkgname}/Kawafile, ${operation}, NULL
    // Kawafiles only need script path and operation and do the rest by themselves, so there's no need for
    // complicated array prepending, NULL appending, type conversion or other stuff
    // it sounds smart right now, but we'll see tomorrow
}

void kawafile_dir_create(char pkgname[]) {
    struct stat st = {0};
    
    char path[strlen(INSTALLPREFIX)+23+strlen(pkgname)];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/kawafiles/");
    strcat(path, pkgname);
    
    if (stat(path, &st) == -1) {
        mkdir(path, 0700);
    }
}
