#include "repoadd.h"

int repoadd(char reponame[], char repourl[]) {
    printf("This will add a repo called %s whose packages.db is located at %s\n", reponame, repourl);
    printf("Do you wish to proceed with this action? [Y/n] ");
    char response = getchar();
    if (response == 'n' || response == 'N')
        return 0;
    else if (response == 'y' || response == 'Y' || response == '\n') {
        printf("Adding repository %s...", reponame);
        fflush(stdout);
        
        FILE *fp;
        
        char path[strlen(INSTALLPREFIX)+23];
        strcpy(path, "");
        strcat(path, INSTALLPREFIX);
        strcat(path, "/etc/kawa.d/repos.conf");
        fp = fopen(path, "a");

        fprintf(fp, "%s %s\n", reponame, repourl);
        
        fclose(fp);
        printf(" Done!\n");
        
        printf("Do you wish to sync this repository now? [Y/n] ");
        fflush(stdout);
        char response = getchar();
        if (response == 'n' || response == 'N')
            return 0;
        else if (response == 'y' || response == 'Y' || response == '\n') {
            printf("Syncing Repo %s...\n", reponame);
            return sync_repo(reponame, repourl);
        } else
            return 1;
    }
    return 1;
}
