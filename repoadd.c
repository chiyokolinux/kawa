/**
 * this file is part of kawa
 * Copyright (c) 2020-2021 Emily <elishikawa@jagudev.net>
 *
 * kawa is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * kawa is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kawa.  If not, see <https://www.gnu.org/licenses/>.
**/

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

        if (!fp) {
            perror("fopen");
            return 3;
        }

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
