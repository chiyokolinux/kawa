#include "database.h"

struct package package_constructor(char* nameParam, char* descriptionParam, char* versionParam, char* archiveurlParam, char* maintainerParam, char* configurecmdParam, char* typeParam, char* sepbuildParam, char* uninstallcmdParam, char* licenseParam) {
    // this constructs a struct package with malloc,
    // so that it can safely returned, even if a local variable was used
    // to initialize some parameter

    // TODO: should be strnlen instead of strlen
    // TODO: I know that this is horrible spaghetti code, please forgive me
    if (!(char *name         = malloc(sizeof(char *) + sizeof(char) * strlen(nameParam         ) + 1))) malloc_fail();
    strcpy(name, nameParam);
    if (!(char *description  = malloc(sizeof(char *) + sizeof(char) * strlen(descriptionParam  ) + 1))) malloc_fail();
    strcpy(description, descriptionParam);
    if (!(char *version      = malloc(sizeof(char *) + sizeof(char) * strlen(versionParam      ) + 1))) malloc_fail();
    strcpy(version, versionParam);
    if (!(char *archiveurl   = malloc(sizeof(char *) + sizeof(char) * strlen(archiveurlParam   ) + 1))) malloc_fail();
    strcpy(archiveurl, archiveurlParam);
    if (!(char *maintainer   = malloc(sizeof(char *) + sizeof(char) * strlen(maintainerParam   ) + 1))) malloc_fail();
    strcpy(maintainer, maintainerParam);
    if (!(char *configurecmd = malloc(sizeof(char *) + sizeof(char) * strlen(configurecmdParam ) + 1))) malloc_fail();
    strcpy(configurecmd, configurecmdParam);
    if (!(char *type         = malloc(sizeof(char *) + sizeof(char) * strlen(typeParam         ) + 1))) malloc_fail();
    strcpy(type, typeParam);
    if (!(char *sepbuild     = malloc(sizeof(char *) + sizeof(char) * strlen(sepbuildParam     ) + 1))) malloc_fail();
    strcpy(sepbuild, sepbuildParam);
    if (!(char *uninstallcmd = malloc(sizeof(char *) + sizeof(char) * strlen(uninstallcmdParam ) + 1))) malloc_fail();
    strcpy(uninstallcmd, uninstallcmdParam);
    if (!(char *license      = malloc(sizeof(char *) + sizeof(char) * strlen(licenseParam      ) + 1))) malloc_fail();
    strcpy(license, licenseParam);

    // copy every pointer to struct object
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
    
    // count spaces
    for (int i = 0; to_split[i]; i++) {
        if (to_split[i] == ' ') {
            spacecount++;
        }
    }
    
    // alloc memory
    char **retval = malloc(sizeof(char *) * (spacecount + 1));
    
    // end at newline
    size_t ln = strlen(p) - 1;
    if (p[ln] == '\n')
        p[ln] = '\0';
    
    // tokenize everything and put it into the array
    int i = 0;
    while (1) {
        char *p2 = strchr(p, ' ');
        if (p2 != NULL)
            *p2 = '\0';
        retval[i] = malloc(sizeof(char[64]));
        // important, do not remove
        strcpy(retval[i], p);
        if (p2 == NULL)
            break;
        p = p2 + 1;
        i++;
    }
    
    struct strarr_retval sarv = { .retval = retval, .retc = spacecount+1 };
    return sarv;
}

void parse_csv_line(char line[], struct package* retval) {
    // see split_space for explanation
    char *p = line;
    size_t ln = strlen(p) - 1;
    char parsed[14][2048];
    if (p[ln] == '\n')
        p[ln] = '\0';
    int index = 0;
    while (1) {
        char *p2 = strchr(p, ';');
        if (p2 != NULL)
            *p2 = '\0';
        strcpy(parsed[index], p);
        if (p2 == NULL)
            break;
        p = p2 + 1;
        index++;
    }
    
    // construct the package
    *retval = package_constructor(parsed[0],parsed[1],parsed[2],parsed[3],parsed[4],parsed[7],parsed[9],parsed[10],parsed[11],parsed[12]);

        retval->depends        = split_space(parsed[5]);
        retval->conflicts      = split_space(parsed[6]);
        retval->configureopts  = split_space(parsed[8]);
        retval->scripts        = split_space(parsed[13]);
}

struct pkglist *get_packages_from_repo(char reponame[]) {
    // init path variable
    char path[strlen(INSTALLPREFIX)+25+strlen(reponame)];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/");
    strcat(path, reponame);
    strcat(path, ".packages.db");
    FILE* indexfile = fopen(path, "r");
    
    // get file size (for malloc)
    fseek(indexfile, 0L, SEEK_END);
    int fsize = ftell(indexfile);
    fseek(indexfile, 0L, SEEK_SET);
    struct package **packages = malloc(sizeof(struct package*) + sizeof(char) * fsize);
    int pkg_count = 0;
    
    // parse every package
    char buffer[4096];
    while (fgets(buffer, 4096, (FILE*)indexfile) != NULL) {
        struct package *newpkg = malloc(sizeof(struct package*) + (sizeof(char *) * 15) + sizeof(struct strarr_retval *) * 5);
        parse_csv_line(buffer, newpkg);
        packages[pkg_count++] = newpkg;
    }

    // construct return value struct pkglist
    struct pkglist *retval = malloc(sizeof(struct pkglist*) + sizeof(char) * fsize);
    retval->pkg_count = pkg_count;
    retval->packages = packages;
    
    fclose(indexfile);
    
    return retval;
}

struct pkglist *get_all_packages() {
    // all repos together probably won't exceed 16MiB. If they do, I'll just release a kawa update making this limit bigger
    // I just checked, 16MiB could cover more than 45,000 packages. Yeah, 16MiB is enough
    struct package **packages = malloc(sizeof(struct pkglist*) + sizeof(char) * 4096 * 4096);
    int pkg_count = 0;
    long total_size = 0L;

    FILE *fp;
    char reponame[127];
    char repourl[511];

    // init path variable
    char path[strlen(INSTALLPREFIX)+23];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/repos.conf");
    fp = fopen(path, "r");

    // go through every repo and parse it using get_packages_from_repo
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
    
    // construct return value
    struct pkglist *retval = malloc(sizeof(struct pkglist*) + total_size);
    retval->pkg_count = pkg_count;
    retval->packages = packages;
    
    return sort_package_list(retval);
}

struct pkglist *get_installed_packages() {
    printf("Querying installed packages...\n");
    return sort_package_list(get_packages_from_repo("Installed"));
}

int compare_strings(const void* a, const void* b) {
    struct package *pkgA = *(struct package**)a;
    struct package *pkgB = *(struct package**)b;
    return strcmp(pkgA->name, pkgB->name); 
} 

struct pkglist *sort_package_list(struct pkglist *orig_pkglist) {
    // this sorts the list of packages (to make a lot of things involving all pkgs vs installed pkgs a lot easier)
    qsort(orig_pkglist->packages, orig_pkglist->pkg_count, sizeof(orig_pkglist->packages[0]), compare_strings);
    return orig_pkglist;
}

void pkglist_free(struct pkglist *packages) {
    for (int i = 0; i < packages->pkg_count; i++) {
        // this should properly free a struct pkglist (i think?)
        struct package *currpkg = packages->packages[i];
        free(currpkg->name);
        free(currpkg->description);
        free(currpkg->version);
        free(currpkg->archiveurl);
        free(currpkg->maintainer);
        free(currpkg->configurecmd);
        free(currpkg->type);
        free(currpkg->sepbuild);
        free(currpkg->uninstallcmd);
        free(currpkg->license);
        free(currpkg->depends.retval);
        free(currpkg->conflicts.retval);
        free(currpkg->configureopts.retval);
        free(currpkg->scripts.retval);
        free(currpkg);
    }
}

int write_installed_packages(struct pkglist *installed) {
    // no sort, cause already and will be done on read
    // speed should only be a problem when you literally install the entire
    // chiyoko linux repo (and also I'll optimize this later)
    
    // init path variable
    char path[strlen(INSTALLPREFIX)+34];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/Installed.packages.db");
    FILE* indexfile = fopen(path, "w");
    
    int retval = 0;
    
    // iterate through all installed packages
    for (int i = 0; i < installed->pkg_count; i++) {
        struct package *currpkg = installed->packages[i];
        // this could've been done by remove, if so, skip package
        // no writing NULL pointers
        if (currpkg == NULL)
            continue;
        // write the package to the index file
        retval += fprintf(indexfile, "%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s\n", currpkg->name, currpkg->description, currpkg->version, currpkg->archiveurl, currpkg->maintainer, whitespace_join(currpkg->depends), whitespace_join(currpkg->conflicts), currpkg->configurecmd, whitespace_join(currpkg->configureopts), currpkg->type, currpkg->sepbuild, currpkg->uninstallcmd, currpkg->license, whitespace_join(currpkg->scripts));
    }
    
    fclose(indexfile);
    
    return retval;
}
