#include "database.h"
#include <errno.h>

struct strarr_retval split_space(char to_split[]) {
    char *p = to_split;
    int spacecount = 0;
    for(int i = 0; to_split[i]; i++) {
        if(to_split[i] == ' ') {
            spacecount++;
        }
    }
    
    char *retval[64];
    // retval = malloc(sizeof(char[64]) * spacecount);
    
    size_t ln = strlen(p) - 1;
    if (p[ln] == '\n')
        p[ln] = '\0';
    
    int i = 0;
    while (1) {
        char *p2 = strchr(p, ' ');
        if(p2 != NULL)
            *p2 = '\0';
        retval[i] = p;
        strcpy(retval[i++], p);
        if(p2 == NULL)
            break;
        p = p2 + 1;
        i++;
    }
    
    struct strarr_retval sarv = { .retval = retval, .retc = spacecount+1 };
    return sarv;
}

struct package parse_csv_line(char line[]) {
    char *p = line;
    size_t ln = strlen(p) - 1;
    char parsed[14][2048];
    if (p[ln] == '\n')
        p[ln] = '\0';
    int index = 0;
    while (1) {
        char *p2 = strchr(p, ';');
        if(p2 != NULL)
            *p2 = '\0';
        strcpy(parsed[index], p);
        if(p2 == NULL)
            break;
        p = p2 + 1;
        index++;
    }
    // struct package *retval = malloc(sizeof(struct package*) + (sizeof(char) * strlen(line)));
    // struct package *retval = &(struct package) {
    struct package retval = {
        .name = parsed[0],
        .description = parsed[1],
        .version = parsed[2],
        .archiveurl = parsed[3],
        .maintainer = parsed[4],
        .depends = split_space(parsed[5]),
        .conflicts = split_space(parsed[6]),
        .configurecmd = parsed[7],
        .configureopts = split_space(parsed[8]),
        .type = parsed[9],
        .sepbuild = parsed[10],
        .uninstallcmd = parsed[11],
        .license = parsed[12],
        .scripts = split_space(parsed[13])
    };
    return retval;
}

struct pkglist get_packages_from_repo(char reponame[]) {
    char path[strlen(INSTALLPREFIX)+25+strlen(reponame)];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/");
    strcat(path, reponame);
    strcat(path, ".packages.db");
    FILE* indexfile = fopen(path, "r");
    
    fseek(indexfile, 0L, SEEK_END);
    int fsize = ftell(indexfile);
    fseek(indexfile, 0L, SEEK_SET);
    struct pkglist *retval = malloc(sizeof(struct pkglist*) + sizeof(char) * fsize);
    retval->pkg_count = 0;
    
    char buffer[4096];
    while (fgets(buffer, 4096, (FILE*)indexfile) != NULL) {
        retval->packages[retval->pkg_count++] = parse_csv_line(buffer);
        // printf("parsed package %s\n", retval.packages[retval.pkg_count-1].name);
    }
    
    fclose(indexfile);
        
    return *retval;
}

struct pkglist get_all_packages() {
    struct pkglist retval;
    retval.pkg_count = 0;

    FILE *fp;
    char reponame[127];
    char repourl[511];

    char path[strlen(INSTALLPREFIX)+23];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/repos.conf");
    fp = fopen(path, "r");

    while (fscanf(fp, "%s %s", reponame, repourl) != EOF) {
        printf("Reading Repo %s...\n", reponame);
        struct pkglist currepo = get_packages_from_repo(reponame);
        for (int i = 0; i < currepo.pkg_count; i++) {
            retval.packages[retval.pkg_count++] = currepo.packages[i];
        }
    }
    
    fclose(fp);
        
    return sort_package_list(retval);
}

struct pkglist get_installed_packages() {
    printf("Querying installed packages...\n");
    return sort_package_list(get_packages_from_repo("installed"));
}

int compare_strings(const void* a, const void* b) {
    struct package *pkgA = (struct package*)a;
    struct package *pkgB = (struct package*)b;
    return strcmp(pkgA->name, pkgB->name); 
} 

struct pkglist sort_package_list(struct pkglist orig_pkglist) {
    qsort(orig_pkglist.packages, orig_pkglist.pkg_count, sizeof(struct pkglist), compare_strings);
    return orig_pkglist;
}
