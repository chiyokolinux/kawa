#include "database.h"

struct package package_constructor(char* nameParam, char* descriptionParam, char* versionParam, char* archiveurlParam, char* maintainerParam, char* configurecmdParam, char* typeParam, char* sepbuildParam, char* uninstallcmdParam, char* licenseParam) {

    // TODO: should be strnlen insteaf of srtlen
    // TODO: I know that this is horrible spaghetti code, please forgive me
    char *name         = malloc(sizeof(char) * strlen(nameParam         ) + 1);
    strcpy(name, nameParam);
    char *description  = malloc(sizeof(char) * strlen(descriptionParam  ) + 1);
    strcpy(description, descriptionParam);
    char *version      = malloc(sizeof(char) * strlen(versionParam      ) + 1);
    strcpy(version, versionParam);
    char *archiveurl   = malloc(sizeof(char) * strlen(archiveurlParam   ) + 1);
    strcpy(archiveurl, archiveurlParam);
    char *maintainer   = malloc(sizeof(char) * strlen(maintainerParam   ) + 1);
    strcpy(maintainer, maintainerParam);
    char *configurecmd = malloc(sizeof(char) * strlen(configurecmdParam ) + 1);
    strcpy(configurecmd, configurecmdParam);
    char *type         = malloc(sizeof(char) * strlen(typeParam         ) + 1);
    strcpy(type, typeParam);
    char *sepbuild     = malloc(sizeof(char) * strlen(sepbuildParam     ) + 1);
    strcpy(sepbuild, sepbuildParam);
    char *uninstallcmd = malloc(sizeof(char) * strlen(uninstallcmdParam ) + 1);
    strcpy(uninstallcmd, uninstallcmdParam);
    char *license      = malloc(sizeof(char) * strlen(licenseParam      ) + 1);
    strcpy(license, licenseParam);

    struct package retval = (struct package) {
        .name         = name          ,
        .description  = description   ,
        .version      = version       ,
        .archiveurl   = archiveurl    ,
        .maintainer   = maintainer    ,
        .configurecmd = configurecmd  ,
        .type         = type          ,
        .sepbuild     = sepbuild      ,
        .uninstallcmd = uninstallcmd  ,
        .license      = license       
    };

    return retval;
}

struct strarr_retval split_space(char to_split[]) {
    char *p = to_split;
    int spacecount = 0;
    for(int i = 0; to_split[i]; i++) {
        if(to_split[i] == ' ') {
            spacecount++;
        }
    }
    
    char **retval = malloc(sizeof(char[64]) * spacecount);
    
    size_t ln = strlen(p) - 1;
    if (p[ln] == '\n')
        p[ln] = '\0';
    
    int i = 0;
    while (1) {
        char *p2 = strchr(p, ' ');
        if(p2 != NULL)
            *p2 = '\0';
        retval[i] = malloc(sizeof(char[64]));
        strcpy(retval[i], p);
        if(p2 == NULL)
            break;
        p = p2 + 1;
        i++;
    }
    
    struct strarr_retval sarv = { .retval = retval, .retc = spacecount+1 };
    return sarv;
}

void parse_csv_line(char line[], struct package* retval) {
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
    
    *retval = package_constructor(parsed[0],parsed[1],parsed[2],parsed[3],parsed[4],parsed[7],parsed[9],parsed[10],parsed[11],parsed[12]);

        retval->depends        = split_space(parsed[5]);
        retval->conflicts      = split_space(parsed[6]);
        retval->configureopts  = split_space(parsed[8]);
        retval->scripts        = split_space(parsed[13]);
}

struct pkglist *get_packages_from_repo(char reponame[]) {
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
    // struct pkglist *retval = malloc(sizeof(struct pkglist*) + sizeof(char) * fsize);
    struct package **packages = malloc(sizeof(struct package*) + sizeof(char) * fsize);
    int pkg_count = 0;
    
    char buffer[4096];
    while (fgets(buffer, 4096, (FILE*)indexfile) != NULL) {
        struct package *newpkg = malloc(sizeof(struct package*) + (sizeof(char) * strlen(buffer)));
        parse_csv_line(buffer, newpkg);
        packages[pkg_count++] = newpkg;
    }

    struct pkglist *retval = malloc(sizeof(struct pkglist*) + sizeof(char) * fsize);
    retval->pkg_count = pkg_count;
    retval->packages = packages;
    // memcpy(retval->packages, packages, sizeof(struct package*) + sizeof(char) * fsize);
    
    fclose(indexfile);
    
    return retval;
}

struct pkglist *get_all_packages() {
    // all repos together probably won't exceed 16MiB. If they do, I'll just release a kawa update making this limit bigger
    // I just checked, 32MiB could cover more than 45,000 packages. Yeah, 16MiB is enough
    struct package **packages = malloc(sizeof(struct pkglist*) + sizeof(char) * 4096 * 4096);
    int pkg_count = 0;
    long total_size = 0L;

    FILE *fp;
    char reponame[127];
    char repourl[511];

    char path[strlen(INSTALLPREFIX)+23];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/repos.conf");
    fp = fopen(path, "r");

    while (fscanf(fp, "%s %s", reponame, repourl) != EOF) {
        printf("Reading Repo %s...", reponame);
        fflush(stdout);
        struct pkglist *currepo = get_packages_from_repo(reponame);
        printf(" Read %d packages\n", currepo->pkg_count);
        for (int i = 0; i < currepo->pkg_count; i++) {
            packages[pkg_count++] = currepo->packages[i];
            total_size += sizeof(currepo->packages[i]);
        }
    }
    
    fclose(fp);
    
    struct pkglist *retval = malloc(sizeof(struct pkglist*) + total_size);
    retval->pkg_count = pkg_count;
    retval->packages = packages;
    // memcpy(retval->packages, packages, total_size);
    
    return sort_package_list(retval);
}

struct pkglist *get_installed_packages() {
    printf("Querying installed packages...\n");
    return sort_package_list(get_packages_from_repo("Installed"));
}

int compare_strings(const void* a, const void* b) {
    struct package *pkgA = *(struct package**)a;
    struct package *pkgB = *(struct package**)b;
    /* printf(" %s ", pkgA->name); */
    return strcmp(pkgA->name, pkgB->name); 
} 

struct pkglist *sort_package_list(struct pkglist *orig_pkglist) {
    qsort(orig_pkglist->packages, orig_pkglist->pkg_count, sizeof(orig_pkglist->packages[0]), compare_strings);
    return orig_pkglist;
}
