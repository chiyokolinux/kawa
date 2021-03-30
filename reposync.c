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

#include "reposync.h"

int sync_repo(char reponame[], char repourl[]) {
    CURL *curl;
    CURLcode res;
    int retval = 0;

    curl = curl_easy_init();
    if (curl) {
        /* curl did some caching of the repos, we don't want that */
        char repourl_cachefix[strlen(repourl) + 20]; // 19 is max value length of unsigned long long, we add a ? so 20 chars max. added
        strcpy(repourl_cachefix, "");
        sprintf(repourl_cachefix, "%s?%ld", repourl, (unsigned long)time(NULL));
        
        curl_easy_setopt(curl, CURLOPT_URL, repourl_cachefix);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        
        char path[strlen(INSTALLPREFIX)+25+strlen(reponame)];
        strcpy(path, "");
        strcat(path, INSTALLPREFIX);
        strcat(path, "/etc/kawa.d/");
        strcat(path, reponame);
        strcat(path, ".packages.db");
        FILE* indexfile = fopen(path, "w+");
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, indexfile);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "Syncing repo %s failed: %s\n", reponame, curl_easy_strerror(res));
            retval++;
        }

        curl_easy_cleanup(curl);
        fclose(indexfile);
    } else {
        fprintf(stderr, "Syncing repo %s failed: Cannot create cURL object\n", reponame);
        return ++retval;
    }
 
    return retval;
}

int sync_all() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    int retval = 0;

    FILE *fp;
    char reponame[127];
    char repourl[511];

    char path[strlen(INSTALLPREFIX)+23];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/repos.conf");
    fp = fopen(path, "r");

    while (fscanf(fp, "%126s %510s", reponame, repourl) != EOF) {
        printf("Syncing Repo %s...\n", reponame);
        retval += sync_repo(reponame, repourl);
    }
    
    fclose(fp);
    
    curl_global_cleanup();
    
    return retval;
}

int sync_repo_cli(char reponame_target[]) {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    int retval = 0, match_found = 0, closest_offset = 2147483647;

    FILE *fp;
    char reponame[127];
    char repourl[511];
    char closest_reponame[127];

    char path[strlen(INSTALLPREFIX)+23];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/repos.conf");
    fp = fopen(path, "r");

    while (fscanf(fp, "%126s %510s", reponame, repourl) != EOF) {
        int name_offset = strcmp(reponame, reponame_target);
        if (name_offset == 0) {
            printf("Syncing Repo %s...\n", reponame);
            retval += sync_repo(reponame, repourl);
            match_found = 1;
        } else if (name_offset < closest_offset) {
            closest_offset = name_offset;
            strncpy(closest_reponame, reponame, 126);
            closest_reponame[126] = '\0';
        }
    }

    if (!match_found) {
        fprintf(stderr, "Syncing repo %s failed: No such repository\nDid you mean: %s ?\n", reponame_target, closest_reponame);
    }

    fclose(fp);

    curl_global_cleanup();

    return 0;
}
