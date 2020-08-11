#include <stdio.h>
#include <curl/curl.h>

int sync_repo(char reponame[], char repourl[]);
int sync_all();

int sync_repo(char reponame[], char repourl[]){
    CURL *curl;
    CURLcode res;
    int retval = 0;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, repourl);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        
        FILE* indexfile = fopen(strcat(INSTALLPREFIX, strcat("/etc/kawa.d/", strcat(reponame, ".packages.db"))), "w+");
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, indexfile) ;

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

    fp = fopen(strcat(INSTALLPREFIX, "/etc/kawa.d/repos.conf"), "r");

    while (fscanf(fp, "%s %s", reponame, repourl) != NULL) {
        printf("Syncing Repo %s...\n", reponame);
        retval += sync_repo(reponame, repourl);
    }
    
    fclose(fp);
    
    curl_global_cleanup();
    
    return retval;
}