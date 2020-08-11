#include <stdio.h>
#include <curl/curl.h>

int sync_repo(char repourl[]

int sync_repo(char repourl[]){
    CURL *curl;
    CURLcode res;


    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, repourl);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return 0;
}

int sync_all() {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    FILE *fp;
    char buff[255];

    fp = fopen(INSTALLPREFIX + "/etc/kawa.d/repos.conf", "r");
    fscanf(fp, "%s", buff);
    printf("1 : %s\n", buff );

    fgets(buff, 255, (FILE*)fp);
    printf("2: %s\n", buff );

    fgets(buff, 255, (FILE*)fp);
    printf("3: %s\n", buff );
    fclose(fp);
    
}
