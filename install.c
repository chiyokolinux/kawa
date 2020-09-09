#include "install.h"

int install(int pkgc, char *pkgnames[]) {
    int resolve_depends = 1;
    int sync_before_install = 0;
    int force_install = 0;
    for (int i = 0; i < pkgc; i++) {
        if (!strcmp(pkgnames[i], "-D") || !strcmp(pkgnames[i], "--no-depends"))
            resolve_depends = 0;
        if (!strcmp(pkgnames[i], "-S") || !strcmp(pkgnames[i], "--sync"))
            sync_before_install = 1;
        if (!strcmp(pkgnames[i], "-r") || !strcmp(pkgnames[i], "--reinstall") || !strcmp(pkgnames[i], "--force"))
            force_install = 1;
    }
    if (sync_before_install)
        sync_all();
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
    if (resolve_depends) {
        for (int i = 2; i < pkgc; i++) {
            resolve_recursive(nodelist, updatepkgs, pkgnames[i], database, installed, 0, updatec, force_install);
        }
    } else {
        for (int ii = 2; ii < pkgc; ii++) {
            if (pkgnames[ii][0] == '-')
                continue;
            struct package *currpkg;
            int success = 0;
            for (int i = 0; i < database->pkg_count; i++) {
                currpkg = database->packages[i];
                if (!strcmp(currpkg->name, pkgnames[ii])) {
                    int current_installed = 0;
                    if (!force_install) {
                        for (int i2 = 0; i2 < installed->pkg_count; i2++) {
                            if (!strcmp(pkgnames[ii], installed->packages[i2]->name)) {
                                current_installed = 1;
                                break;
                            }
                        }
                    }
                    if (current_installed) {
                        struct pkg_update *pkgupdt = pkg_has_update(pkgnames[ii], database, installed);
                        if (pkgupdt != NULL) {
                            updatepkgs[*updatec] = pkgupdt;
                            *updatec = *updatec + 1;
                        }
                    }
                    if (!current_installed) {
                        nodelist->packages[nodelist->pkg_count] = currpkg;
                        nodelist->pkg_count++;
                    }
                    success = 1;
                }
            }
            if (!success) {
                printf("Error: Package %s not found.\n", pkgnames[ii]);
                printf("This error may be solved by kawa sync, but if not, please contact the maintainer of the package you want to install.\n");
                exit(-3);
            }
        }
    }
    if (!nodelist->pkg_count && !*updatec) {
        printf("All packages are installed and all dependencies are satisfied.\n");
        return 0;
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
            // because we're updating, nothing's being changed anyways, so we can just leave manual_installed be false.
            retval += install_no_deps(updatepkgs[i]->name, database, 0);
        }

        for (int i = 0; i < nodelist->pkg_count; i++) {
            int maninst = 0;
            for (int i2 = 0; i2 < pkgc; i2++) {
                if (!strcmp(nodelist->packages[i]->name, pkgnames[i2])) {
                    maninst = 1;
                    break;
                }
            }
            retval += install_no_deps(nodelist->packages[i]->name, database, maninst);
        }

        curl_global_cleanup();
        return retval;
    } else
        return 1;
}

int download_archive(struct package *dlpackage, char filetype[]) {
    printf("Downloading %s...", dlpackage->name);
    fflush(stdout);
    
    struct stat st = {0};
    
    char path[strlen(INSTALLPREFIX)+44+strlen(dlpackage->name)];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/kawafiles/");
    strcat(path, dlpackage->name);
    strcat(path, "/package.tar.");
    strcat(path, filetype);
    
    if (stat(path, &st) != -1) {
        printf(" Done\n");
        return 0;
    }
    
    CURL *curl;
    CURLcode res;
    int retval = 0;

    curl = curl_easy_init();
    if (curl) {
        char *archiveurl_version_replaced = str_replace(dlpackage->archiveurl, "${VERSION}", dlpackage->version);
        char *archiveurl_name_replaced = str_replace(archiveurl_version_replaced, "${NAME}", dlpackage->name);
        curl_easy_setopt(curl, CURLOPT_URL, archiveurl_name_replaced);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        
        
        FILE* indexfile = fopen(path, "w+");
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, indexfile);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "Downloading package %s failed: %s\n", dlpackage->name, curl_easy_strerror(res));
            retval++;
        }

        curl_easy_cleanup(curl);
        fclose(indexfile);
    } else {
        fprintf(stderr, "Downloading package %s failed: Cannot create cURL object\n", dlpackage->name);
        return ++retval;
    }

    printf(" Done\n");
    return retval;
}

int install_no_deps(char pkgname[], struct pkglist *database, int manual_installed) {
    struct package *currpkg;
    for (int i = 0; i < database->pkg_count; i++) {
        currpkg = database->packages[i];
        if (!strcmp(currpkg->name, pkgname)) {
            // compute filetype
            char *p = malloc(strlen(currpkg->archiveurl)+1);
            strcpy(p, currpkg->archiveurl);
            size_t ln = strlen(p) - 1;
            char filetype[8];
            if (p[ln] == '\n')
                p[ln] = '\0';
            while (1) {
                char *p2 = strchr(p, '.');
                if(p2 != NULL)
                    *p2 = '\0';
                if (strlen(p) < 8)
                    strcpy(filetype, p);
                if(p2 == NULL)
                    break;
                p = p2 + 1;
            }
            
            add_db_entry(currpkg, manual_installed);
            kawafile_dir_create(pkgname);
            
            if (download_archive(currpkg, filetype))
                return 1;
            if (!strcmp(currpkg->type, "source"))
                return 0; // TODO: sourcepkg_install(name=pkgname)
            else if (!strcmp(currpkg->type, "patch"))
                return 0; // TODO: sourcepkg_install(patch=pkgname)
            else if (!strcmp(currpkg->type, "meta"))
                return metapkg_install(pkgname);
            else if (!strcmp(currpkg->type, "binary"))
                return binarypkg_install(pkgname, filetype);
            return 1;
        }
    }
    printf("Error: Package %s not found (try kawa sync)\n", pkgname);
    return 1;
}

int add_db_entry(struct package *package, int manual_installed) {
    // files are for tomorrow
    
    struct stat st = {0};
    
    char path[strlen(INSTALLPREFIX)+23+strlen(package->name)];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/kawafiles/");
    strcat(path, package->name);
    
    if (stat(path, &st) != -1) {
        return 0;
    }
    
    char indexpath[strlen(INSTALLPREFIX)+34];
    strcpy(indexpath, "");
    strcat(indexpath, INSTALLPREFIX);
    strcat(indexpath, "/etc/kawa.d/Installed.packages.db");

    FILE* indexfile = fopen(indexpath, "a+");
    
    char manual[9];
    if (manual_installed)
        strcpy(manual, "manual");
    else
        strcpy(manual, "auto");
    
    fprintf(indexfile, "%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s\n", package->name, manual, package->version, package->archiveurl, package->maintainer, whitespace_join(package->depends), whitespace_join(package->conflicts), package->configurecmd, whitespace_join(package->configureopts), package->type, package->sepbuild, package->uninstallcmd, package->license, whitespace_join(package->scripts));
    
    fclose(indexfile);
    return 0;
}

char *str_replace(char *orig, char *rep, char *with) {
    char *result;
    char *ins;
    char *tmp;
    int len_rep;
    int len_with;
    int len_front;
    int count;
    
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL;
    if (!with)
        with = "";
    len_with = strlen(with);
    
    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }
    
    tmp = result = malloc(strlen(orig) * sizeof(char) + (len_with - len_rep) * count * sizeof(char) + 1);

    if (!result)
        return NULL;

    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep;
    }
    strcpy(tmp, orig);
    return result;
}

char *whitespace_join(struct strarr_retval to_join) {
    int charc = 0;
    for (int i = 0; i < to_join.retc; i++)
        charc += strlen(to_join.retval[i]);
    char *joined = malloc(sizeof(char) * charc + sizeof(char) * to_join.retc);
    strcpy(joined, ""); // this seems to fix some stuff
    for (int i = 0; i < to_join.retc; i++) {
        strcat(joined, to_join.retval[i]);
        if (i != to_join.retc - 1)
            strcat(joined, " ");
    }
    return joined;
}
