#include "reposync.h"

int sync_repo(char reponame[], char repourl[]) {
    CURL *curl;
    CURLcode res;
    int retval = 0;

    curl = curl_easy_init();
    if (curl) {
        /* curl did some caching of the repos, we don't want that */
        char repourl_cachefix[strlen(repourl) + 20]; // 19 is max value of unsigned long long, we add a ? so 20 chars max. added
        strcpy(repourl_cachefix, "");
        sprintf(repourl_cachefix, "%s?%ld", repourl, (unsigned long)time(NULL));
        
        curl_easy_setopt(curl, CURLOPT_URL, repourl_cachefix);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        
        // struct curl_slist *list = NULL;
        // list = curl_slist_append(list, "Cache-Control: no-cache");
        // list = curl_slist_append(list, "Pragma: no-cache");
        // curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
        
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
        // curl_slist_free_all(list);
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

    while (fscanf(fp, "%s %s", reponame, repourl) != EOF) {
        printf("Syncing Repo %s...\n", reponame);
        retval += sync_repo(reponame, repourl);
    }
    
    fclose(fp);
    
    curl_global_cleanup();
    
    return retval;
}
