#include "install.h"

int install(int pkgc, char *pkgnames[]) {
    struct pkglist *database = get_all_packages();
    struct pkglist *installed = get_installed_packages();
    struct pkg_update *updatepkgs[installed->pkg_count];
    struct package **packages = malloc(sizeof(char)*database->pkg_count*1024);
    struct pkglist *nodelist = malloc(sizeof(struct pkglist)+sizeof(char)*database->pkg_count*1024);
    nodelist->pkg_count = 0;
    nodelist->packages = packages;
    int *updatec = malloc(sizeof(int));
    *updatec = 0;
    printf("\n");
    for (int i = 2; i < pkgc; i++) {
        resolve_recursive(nodelist, updatepkgs, pkgnames[i], database, installed, 0, updatec);
    }
    printf("The following packages will be installed:\n ");
    for (int i = 0; i < nodelist->pkg_count; i++) {
        printf(" %s", nodelist->packages[i]->name);
    }
    if (*updatec) {
        printf("\nThe following packages will be updated:\n ");
        for (int i = 0; i < *updatec; i++) {
            printf(" %s (%s -> %s)", updatepkgs[i]->name, updatepkgs[i]->version_local, updatepkgs[i]->version_remote);
        }
    }
    printf("\n\n%d package(s) will be installed, %d package(s) will be removed and %d package(s) will be updated.\n", nodelist->pkg_count, 0, *updatec);
    printf("Do you wish to proceed? [Y/n] ");
    fflush(stdout);
    char response = getchar();
    if (response == 'n' || response == 'N')
        return 0;
    else if (response == 'y' || response == 'Y' || response == '\n') {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        int retval = 0;
        for (int i = 0; i < *updatec; i++) {
            retval += install_no_deps(updatepkgs[i]->name, database);
        }
        for (int i = 0; i < nodelist->pkg_count; i++) {
            retval += install_no_deps(nodelist->packages[i]->name, database);
        }
            curl_global_cleanup();
        return retval;
    } else
        return 1;
}

int download_archive(struct package *dlpackage) {
    
    
    CURL *curl;
    CURLcode res;
    int retval = 0;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, dlpackage->archiveurl);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        
        char path[strlen(INSTALLPREFIX)+25+strlen(dlpackage->name)];
        strcpy(path, "");
        strcat(path, INSTALLPREFIX);
        strcat(path, "/etc/kawa.d/kawafiles/");
        strcat(path, dlpackage->name);
        strcat(path, "/archive.tar.TODO: detect file type");
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

int install_no_deps(char pkgname[], struct pkglist *database) {
    struct package *currpkg;
    for (int i = 0; i < database->pkg_count; i++) {
        currpkg = database->packages[i];
        if (download_archive(currpkg))
            return 1;
        if (!strcmp(currpkg->name, pkgname)) {
            if (!strcmp(currpkg->type, "source"))
                return 0; // TODO: sourcepkg_install(name=pkgname)
            else if (!strcmp(currpkg->type, "patch"))
                return 0; // TODO: sourcepkg_install(patch=pkgname)
            else if (!strcmp(currpkg->type, "meta"))
                return metapkg_install(pkgname);
            else if (!strcmp(currpkg->type, "binary"))
                return binarypkg_install(pkgname);
            return 1;
        }
    }
    printf("Error: Package %s not found (try kawa sync)\n", pkgname);
    return 1;
}
