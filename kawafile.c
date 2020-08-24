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
