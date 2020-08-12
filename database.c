#include "database.h"

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
    printf("4\n");
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
    printf("5\n");
    struct package retval = {
        .name = parsed[0],
        .description = parsed[1],
        .version = parsed[2],
        .archiveurl = parsed[3],
        .maintainer = parsed[4],
        .depends = split_space(parsed[5]).retval,
        .conflicts = split_space(parsed[6]).retval,
        .configurecmd = parsed[7],
        .configureopts = split_space(parsed[8]).retval,
        .type = parsed[9],
        .sepbuild = parsed[10],
        .uninstallcmd = parsed[11],
        .license = parsed[12],
        .scripts = split_space(parsed[13]).retval
    };
    printf("6\n");
    return retval;
}

struct pkglist get_packages_from_repo(char reponame[]) {
    printf("7\n");
    struct pkglist retval;
    retval.pkg_count = 0;
    
    struct ll_node curpkg;
    
    char path[strlen(INSTALLPREFIX)+25+strlen(reponame)];
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/");
    strcat(path, reponame);
    strcat(path, ".packages.db");
    FILE* indexfile = fopen(path, "r");
    printf("8\n");
    
    char buffer[4096];
    while (fgets(buffer, 4096, (FILE*)indexfile) != NULL) {
        printf("9\n");
        curpkg.current = parse_csv_line(buffer);;
        curpkg.next->prev = &curpkg;
        curpkg = *(curpkg.next);
    }
    
    printf("10\n");
    /* this is a lazy fix, but i'm too tired to do it properly rn */
    curpkg = *(curpkg.prev);
    curpkg.next = NULL;
    while (curpkg.prev) {
        curpkg = *(curpkg.prev);
    }
    
    printf("11\n");
    /* is this required? idk, don't wanna check (it isn't, it's even illegal, ok) */
    // struct package packages[retval.pkg_count];
    int index = 0;
    while (curpkg.next != NULL) {
        // packages[index++] = curpkg.current;
        retval.packages[index++] = curpkg.current;
        curpkg = *(curpkg.next);
    }
    
    fclose(indexfile);
    
    printf("12\n");
    
    return retval;
}

struct pkglist get_all_packages() {
    printf("13\n");
    struct pkglist retval;
    retval.pkg_count = 0;
    
    struct ll_node curpkg;

    FILE *fp;
    char reponame[127];
    char repourl[511];
    printf("14\n");

    char path[strlen(INSTALLPREFIX)+23];
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/repos.conf");
    fp = fopen(path, "r");
    printf("15\n");
    fgets(repourl, 511, (FILE*)fp);
    printf("17 %s\n", repourl);

    while (fscanf(fp, "%s %s", reponame, repourl) != EOF) {
        printf("16\n");
        printf("Reading Repo %s...\n", reponame);
        struct pkglist currepo = get_packages_from_repo(reponame);
        retval.pkg_count += currepo.pkg_count;
        for (int i = 0; i < currepo.pkg_count; i++) {
            curpkg.current = currepo.packages[i];
            curpkg.next->prev = &curpkg;
            curpkg = *(curpkg.next);
        }
    }
    
    /* this is a lazy fix, but i'm too tired to do it properly rn */
    curpkg = *(curpkg.prev);
    curpkg.next = NULL;
    while (curpkg.prev) {
        curpkg = *(curpkg.prev);
    }
    
    /* is this required? idk, don't wanna check (it isn't, it's even illegal, ok) */
    // struct package packages[retval.pkg_count];
    int index = 0;
    while (curpkg.next != NULL) {
        // packages[index++] = curpkg.current;
        retval.packages[index++] = curpkg.current;
        curpkg = *(curpkg.next);
    }
    
    // retval.packages = packages;
    
    fclose(fp);
        
    return sort_package_list(retval);
}

struct pkglist get_installed_packages() {
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
