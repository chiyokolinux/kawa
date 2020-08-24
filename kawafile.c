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

void kawafile_run(char pkgname[], char *const argv[]) {
    
}
