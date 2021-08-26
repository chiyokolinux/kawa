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

#include "install.h"

int install(int pkgc, char *pkgnames[]) {
    // parse opts
    int resolve_depends = 1, sync_before_install = 0, force_install = 0, ignore_updates = 0, accept_nonfree = 0;
    for (int i = 0; i < pkgc; i++) {
        if (!strcmp(pkgnames[i], "-D") || !strcmp(pkgnames[i], "--no-depends"))
            resolve_depends = 0;
        if (!strcmp(pkgnames[i], "-S") || !strcmp(pkgnames[i], "--sync"))
            sync_before_install = 1;
        if (!strcmp(pkgnames[i], "-r") || !strcmp(pkgnames[i], "--reinstall") || !strcmp(pkgnames[i], "--force"))
            force_install = 1;
        if (!strcmp(pkgnames[i], "-I") || !strcmp(pkgnames[i], "--ignore-updates"))
            ignore_updates = 1;
        if (!strcmp(pkgnames[i], "-N") || !strcmp(pkgnames[i], "--accept-nonfree"))
            accept_nonfree = 1;
    }
    if (sync_before_install)
        sync_all();
    
    // fetch pkglists & init vars
    struct pkglist *database = get_all_packages();
    struct pkglist *installed = get_installed_packages();
    struct pkg_update *updatepkgs[installed->pkg_count];
    struct package **packages;
    if (!(packages = malloc(sizeof(char) * database->pkg_count * 1024))) malloc_fail();
    struct pkglist *nodelist;
    if (!(nodelist = malloc(sizeof(struct pkglist) + 4))) malloc_fail();
    nodelist->pkg_count = 0;
    nodelist->packages = packages;
    struct package **patches;
    if (!(patches = malloc(sizeof(char) * database->pkg_count * 256))) malloc_fail();
    struct pkglist *patchlist;
    if (!(patchlist = malloc(sizeof(struct pkglist) + 4))) malloc_fail();
    patchlist->pkg_count = 0;
    patchlist->packages = patches;
    int *updatec;
    if (!(updatec = malloc(sizeof(int)))) malloc_fail();
    *updatec = 0;
    printf("\n");

    // dependency types
    unsigned int *deptypes;
    if (!(deptypes = malloc(sizeof(unsigned int) * 5))) malloc_fail();
    int current_deptype = 0, current_bytepos = 0, current_char = 0;

    while (DEPTYPES[current_char++] != '\0') {
        if (DEPTYPES[current_char] == ' ') {
            current_deptype++;
            current_bytepos = 0;
        } else {
            deptypes[current_deptype] |= DEPTYPES[current_char] << current_bytepos;
            current_bytepos += 8;
        }
    }
    deptypes[++current_deptype] = 0;

    struct strarr_retval pkg_deptypes = split_space(strdup(DEPTYPES));
    
    // resolve dependencies
    if (resolve_depends) {
        for (int i = 2; i < pkgc; i++) {
            resolve_recursive(nodelist, updatepkgs, patchlist, pkgnames[i], "", database, installed, 0, updatec, force_install, ignore_updates, deptypes);
        }
    } else {
        for (int ii = 2; ii < pkgc; ii++) {
            if (pkgnames[ii][0] == '-')
                continue;

            struct package *currpkg;
            int *i;
            if (!(i = malloc(sizeof(int)))) malloc_fail();
            currpkg = bsearch_pkg(pkgnames[ii], database, i, 0);

            if (!strcmp(currpkg->type, "patch")) {
                fprintf(stderr, "Warning: Installing patch packages without dependency resolution will not recompile the patched packages. Please run kawa install -r -D %s later.\n", currpkg->depends.retval[0]);
            }

            int current_installed = 0;
            if (!force_install) {
                for (int i2 = 0; i2 < installed->pkg_count; i2++) {
                    if (!strcmp(pkgnames[ii], installed->packages[i2]->name)) {
                        current_installed = 1;
                        break;
                    }
                }
            }

            check_package_source(currpkg, *i, database, installed, current_installed);

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

            free(i);
        }
    }

    // check for Non-Free installs and warn the user
    if (!accept_nonfree) {
        for (int i = 0; i < nodelist->pkg_count; i++) {
            if (!strcmp(nodelist->packages[i]->license, "OTHER") || !strcmp(nodelist->packages[i]->license, "NONFREE")) {
                printf("The package %s is licensed under a Non-Free license,\n"
                       "meaning the author might restrict what you can do with the software.\n"
                       "Do you wish to install it anyways? [y/N] ", nodelist->packages[i]->name);
                fflush(stdout);

                char response = getchar();
                if (response == 'n' || response == 'N' || response == '\n') {
                    return 0;
                }
                getchar();
                fflush(stdin);
                printf("\n");
            }
        }
    }
    
    // user dialogue
    if (!nodelist->pkg_count && !patchlist->pkg_count && !*updatec) {
        printf("All packages are installed and all dependencies are satisfied.\n");
        return 0;
    }
    if (nodelist->pkg_count || patchlist->pkg_count) {
        printf("The following packages will be installed:\n ");
        for (int i = 0; i < patchlist->pkg_count; i++) {
            printf(" %s", patchlist->packages[i]->name);
        }
        for (int i = 0; i < nodelist->pkg_count; i++) {
            printf(" %s", nodelist->packages[i]->name);
        }
    }
    if (*updatec) {
        printf("\nThe following packages will be updated:\n ");
        for (int i = 0; i < *updatec; i++) {
            printf(" %s (%s -> %s)", updatepkgs[i]->name, updatepkgs[i]->version_local, updatepkgs[i]->version_remote);
        }
    }
    
    printf("\n\n%d package(s) will be installed, %d package(s) will be removed and %d package(s) will be updated.\n", nodelist->pkg_count + patchlist->pkg_count, 0, *updatec);
    printf("Do you wish to proceed? [Y/n] ");
    fflush(stdout);
    char response = getchar();
    if (response == 'n' || response == 'N')
        return 0;
    else if (response == 'y' || response == 'Y' || response == '\n') {
        return download_install_packages(nodelist, patchlist, updatepkgs, updatec, database, installed, pkg_deptypes, pkgc, pkgnames);
    } else
        return 1;
}

int download_install_packages(struct pkglist *nodelist, struct pkglist *patchlist, struct pkg_update **updatepkgs, int *updatec, struct pkglist *database, struct pkglist *installed, struct strarr_retval pkg_deptypes, int pkgc, char *pkgnames[]) {
    // install everything
    curl_global_init(CURL_GLOBAL_DEFAULT);
    int retval = 0, failc = 0;

    // first, download all package archives and register all packages
    for (int i = 0; i < *updatec; i++) {
        struct package *currpkg;
        int *ii;
        if (!(ii = malloc(sizeof(int)))) malloc_fail();
        currpkg = bsearch_pkg(updatepkgs[i]->name, database, ii, 0);
        free(ii);

        retval = download_package(currpkg, database, 1);

        if (retval) {
            if (failc < MAXDOWNLOADRETRY) {
                fprintf(stderr, "Retrying download of package %s due to above error.\n", updatepkgs[i]->name);
                failc++;
                i--;
            } else {
                fprintf(stderr, "Download of package %s failed. Aborting.\n", updatepkgs[i]->name);
                curl_global_cleanup();
                return retval;
            }
        } else {
            failc = 0;
        }
    }
    for (int i = 0; i < nodelist->pkg_count; i++) {
        retval += download_package(nodelist->packages[i], database, 0);

        if (retval) {
            if (failc < MAXDOWNLOADRETRY) {
                fprintf(stderr, "Retrying download of package %s due to above error.\n", nodelist->packages[i]->name);
                failc++;
                i--;
            } else {
                fprintf(stderr, "Download of package %s failed. Aborting.\n", nodelist->packages[i]->name);
                curl_global_cleanup();
                return retval;
            }
        } else {
            failc = 0;
        }
    }
    for (int i = 0; i < patchlist->pkg_count; i++) {
        retval += download_package(patchlist->packages[i], database, 0);

        if (retval) {
            if (failc < MAXDOWNLOADRETRY) {
                fprintf(stderr, "Retrying download of package %s due to above error.\n", patchlist->packages[i]->name);
                failc++;
                i--;
            } else {
                fprintf(stderr, "Download of package %s failed. Aborting.\n", patchlist->packages[i]->name);
                curl_global_cleanup();
                return retval;
            }
        } else {
            failc = 0;
        }
    }

    // go offline, for long upgrades / upgrades where network software is changed
    curl_global_cleanup();

    // second, we "install" (i.e. unpack & register) all patch packages
    for (int i = 0; i < patchlist->pkg_count; i++) {
        int maninst = 0;
        for (int i2 = 0; i2 < pkgc; i2++) {
            if (!strcmp(patchlist->packages[i]->name, pkgnames[i2])) {
                maninst = 1;
                break;
            }
        }
        retval += install_no_deps(patchlist->packages[i], database, maninst, 0, &pkg_deptypes);
    }

    // then, update packages
    for (int i = 0; i < *updatec; i++) {
        struct package *currpkg;
        int *ii;
        if (!(ii = malloc(sizeof(int)))) malloc_fail();
        currpkg = bsearch_pkg(updatepkgs[i]->name, database, ii, 0);
        free(ii);

        // because we're updating, nothing's being changed anyways, so we can just leave manual_installed be false.
        retval += install_no_deps(currpkg, database, 0, 1, NULL);
        
        // re-wire version variables
        for (int i2 = 0; i2 < installed->pkg_count; i2++) {
            currpkg = installed->packages[i2];
            if (!strcmp(currpkg->name, updatepkgs[i]->name)) {
                currpkg->version = updatepkgs[i]->version_remote;
            }
        }
    }

    // re-write installed if packages were updated
    if (*updatec)
        write_installed_packages(installed, database);

    // after updating, install all new packages
    for (int i = 0; i < nodelist->pkg_count; i++) {
        int maninst = 0;
        for (int i2 = 0; i2 < pkgc; i2++) {
            if (!strcmp(nodelist->packages[i]->name, pkgnames[i2])) {
                maninst = 1;
                break;
            }
        }
        retval += install_no_deps(nodelist->packages[i], database, maninst, 0, &pkg_deptypes);
    }

    return retval;
}

int download_package(struct package *currpkg, struct pkglist *database, int is_update) {
    // register & prepare package
    kawafile_dir_create(currpkg->name);

    // download package & scripts
    if (download_archive(currpkg, is_update, database))
        return 1;
    if (download_scripts(currpkg, database->repos->repos[*currpkg->repoindex]->baseurl))
        return 1;
    return 0;
}

int download_archive(struct package *dlpackage, int force, struct pkglist *database) {
    printf("Downloading %s...", dlpackage->name);
    fflush(stdout);

    struct stat st = {0};

    // initialize path
    char path[strlen(INSTALLPREFIX)+46+strlen(dlpackage->name)];
    strcpy(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/kawafiles/");
    strcat(path, dlpackage->name);
    strcat(path, "/package.src.kawapkg");
    
    // if dir already exists, skip download
    if (stat(path, &st) != -1 && !force) {
        printf(" Done\n");
        return 0;
    }

    // package archive references
    if (!strncmp(dlpackage->archiveurl, "pkg://", 6)) {
        struct package *dlpackage_ref;
        int *i;
        if (!(i = malloc(sizeof(int)))) malloc_fail();
        dlpackage_ref = bsearch_pkg(dlpackage->archiveurl + 6, database, i, 1);
        free(i);

        printf(" (reference to %s)\n", dlpackage_ref->name);

        kawafile_dir_create(dlpackage_ref->name);
        int retval = download_archive(dlpackage_ref, force, database);

        char path_ref[26+strlen(dlpackage_ref->name)];
        strcpy(path_ref, "../");
        strcat(path_ref, dlpackage_ref->name);
        strcat(path_ref, "/package.src.kawapkg");

        struct stat st = {0};
        if (stat(path, &st) != -1) {
            if (!unlink(path)) {
                perror("unlink");
                return 1;
            }
        }

        if (!symlink(path_ref, path)) {
            // we haven't chdir'd to the "correct" directory
            // so symlink wrongly interprets this as a danling link
            // so we need to ignore ENOENT
            if (errno != ENOENT) {
                perror("symlink");
                return 1;
            }
        }

        return retval;
    }

    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        // archiveurl variable parsing (very simple)
        char *archiveurl_version_replaced = str_replace(dlpackage->archiveurl, "${VERSION}", dlpackage->version);
        char *archiveurl_name_replaced = str_replace(archiveurl_version_replaced, "${NAME}", dlpackage->name);
        curl_easy_setopt(curl, CURLOPT_URL, archiveurl_name_replaced);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // do the download thing
        FILE* indexfile = fopen(path, "w+");
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, indexfile);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "\nDownloading package %s failed: %s\n", dlpackage->name, curl_easy_strerror(res));
            fclose(indexfile);
            unlink(path);
            curl_easy_cleanup(curl);
            return 1;
        }

        // check response code
        long respcode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &respcode);
        if (respcode != 200) {
            fprintf(stderr, "\nDownloading package %s failed: HTTP Error %ld\n", dlpackage->name, respcode);
            fclose(indexfile);
            unlink(path);
            curl_easy_cleanup(curl);
            return 2;
        }

        curl_easy_cleanup(curl);
        fclose(indexfile);
    } else {
        fprintf(stderr, "\nDownloading package %s failed: Cannot create cURL object\n", dlpackage->name);
        return 3;
    }

    printf(" Done\n");

    return 0;
}

int download_scripts(struct package *dlpackage, char *baseurl) {
    if (!dlpackage->scripts.retc)
        return 0;
    if (dlpackage->scripts.retval[0][0] == '\0')
        return 0;

    printf("Downloading scripts for package %s.", dlpackage->name);
    fflush(stdout);

    // initialize vars
    char path[strlen(INSTALLPREFIX)+34+strlen(dlpackage->name)];
    char scripturl[strlen(baseurl)+58+strlen(dlpackage->name)];

    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();

    if (!curl) {
        fprintf(stderr, "\nDownloading scripts of package %s failed: Cannot create cURL object\n", dlpackage->name);
        return 3;
    }

    for (int i = 0; i < dlpackage->scripts.retc; i++) {
        char *script = dlpackage->scripts.retval[i];

        if (script[0] == '\0')  // fix for no scripts
            continue;

        // construct current path
        strcpy(path, "");
        strcat(path, INSTALLPREFIX);
        strcat(path, "/etc/kawa.d/kawafiles/");
        strcat(path, dlpackage->name);
        strcat(path, "/");
        strcat(path, script);

        // construct url
        strcpy(scripturl, "");
        strcat(scripturl, baseurl);
        if (scripturl[strlen(scripturl)-1] != '/')
            strcat(scripturl, "/");
        strcat(scripturl, "packages/");
        strcat(scripturl, dlpackage->name);
        strcat(scripturl, "/");
        strcat(scripturl, script);

        curl_easy_setopt(curl, CURLOPT_URL, scripturl);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // do the download thing
        FILE* sfile = fopen(path, "w+");
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, sfile);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "\nDownloading script %s of package %s failed: %s\n", script, dlpackage->name, curl_easy_strerror(res));
            fclose(sfile);
            unlink(path);
            curl_easy_cleanup(curl);
            return 1;
        }

        // check response code
        long respcode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &respcode);
        if (respcode != 200) {
            fprintf(stderr, "\nDownloading script %s of package %s failed: HTTP Error %ld\n", script, dlpackage->name, respcode);
            fclose(sfile);
            unlink(path);
            curl_easy_cleanup(curl);
            return 2;
        }

        if (fchmod(fileno(sfile), S_IRWXU))
            perror("fchmod");
        fclose(sfile);

        printf(".");
        fflush(stdout);
    }

    curl_easy_cleanup(curl);

    
    printf(" Done\n");
    
    return 0;
}

int install_no_deps(struct package *currpkg, struct pkglist *database, int manual_installed, int is_update, struct strarr_retval *deptypes) {
    int retval = 1;

    if (!is_update) {
        if (!strcmp(currpkg->type, "source"))
            retval = sourcepkg_install(currpkg);
        else if (!strcmp(currpkg->type, "patch"))
            retval = patchpkg_install(currpkg);
        else if (!strcmp(currpkg->type, "meta"))
            retval = metapkg_install(currpkg->name);
        else if (!strcmp(currpkg->type, "binary"))
            retval = binarypkg_install(currpkg);
    } else { // if action is update, use the update functions
        if (!strcmp(currpkg->type, "source"))
            retval = sourcepkg_update(currpkg);
        else if (!strcmp(currpkg->type, "patch"))
            retval = patchpkg_update(currpkg);
        else if (!strcmp(currpkg->type, "meta"))
            retval = metapkg_update(currpkg->name);
        else if (!strcmp(currpkg->type, "binary"))
            retval = binarypkg_update(currpkg);
    }

    if (!retval) {
        if (!is_update)
            add_db_entry(currpkg, manual_installed, database, deptypes);
    } else {
        fprintf(stderr, "\nThere were errors during the installation of %s. Aborting.\n", currpkg->name);
        exit(retval);
    }

    return retval;
}

int add_db_entry(struct package *package, int manual_installed, struct pkglist *database, struct strarr_retval *deptypes) {
    // TODO: Write Dependency Types to package->depends field

    // check if already in installed db, i.e. Installed.packages.db
    // that was parsed at program run does NOT contain package.
    // if it does, just return
    // NOTE: we check this earlier now, so this check has been removed
    /*int *i;
    if (!(i = malloc(sizeof(int)))) malloc_fail();
    if (bsearch_pkg(package->name, installed, i, -1)) {
        return 0;
    }*/

    // open file
    char indexpath[strlen(INSTALLPREFIX)+34];
    strcpy(indexpath, "");
    strcat(indexpath, INSTALLPREFIX);
    strcat(indexpath, "/etc/kawa.d/Installed.packages.db");

    FILE* indexfile = fopen(indexpath, "a+");

    // some stuff for detecting orphans later
    char manual[9];
    if (manual_installed)
        strcpy(manual, "manual");
    else
        strcpy(manual, "auto");

    // write entry
    fprintf(indexfile, "%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s\n", package->name, manual, package->version, package->archiveurl, database->repos->repos[*package->repoindex]->reponame, deptypes != NULL ? whitespace_join(*deptypes) : whitespace_join(package->depends), whitespace_join(package->conflicts), package->configurecmd, whitespace_join(package->configureopts), package->type, package->sepbuild, package->uninstallcmd, package->license, whitespace_join(package->scripts));

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
    if (!tmp) malloc_fail();

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
    char *joined;
    if (!(joined = malloc(sizeof(char) * charc + sizeof(char) * to_join.retc))) malloc_fail();
    strcpy(joined, ""); // this seems to fix some stuff
    for (int i = 0; i < to_join.retc; i++) {
        strcat(joined, to_join.retval[i]);
        if (i != to_join.retc - 1)
            strcat(joined, " ");
    }
    return joined;
}
