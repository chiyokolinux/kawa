#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct pkglist get_packages_from_repo(char reponame[]);
struct pkglist get_all_packages();
int compare_strings(const void* a, const void* b);
struct pkglist sort_package_list(struct pkglist orig_pkglist);

struct pkglist get_packages_from_repo(char reponame[]) {
    
}

struct pkglist get_all_packages() {
    struct pkglist retval;
    retval.pkg_count = 0;
    
    struct ll_node curpkg;

    FILE *fp;
    char reponame[127];
    char repourl[511];

    fp = fopen(strcat(INSTALLPREFIX, "/etc/kawa.d/repos.conf"), "r");

    while (fscanf(fp, "%s %s", reponame, repourl) != NULL) {
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

int compare_strings(const void* a, const void* b) {
    struct package *pkgA = (struct package*)a;
    struct package *pkgB = (struct package*)b;
    return strcmp(pkgA->name, pkgB->name); 
} 

struct pkglist sort_package_list(struct pkglist orig_pkglist) {
    qsort(orig_pkglist.packages, orig_pkglist.pkg_count, sizeof(struct pkglist), compare_strings);
    return orig_pkglist;
}
