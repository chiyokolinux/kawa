#include "update.h"

int update() {
    sync_all();
    struct pkglist database = get_all_packages();
    struct pkglist installed = get_installed_packages();
    struct ll_node_update updatepkg;
    int remote_i = 0;
    int updatec = 0;
    for (int i = 0; i < installed.pkg_count; i++) {
        struct package *currpkg_l = installed.packages[i];
        while (strcmp(currpkg_l->name, database.packages[remote_i]->name))
            remote_i++; // this should work because the package lists are sorted (hopefully)
        if (strcmp(currpkg_l->version, database.packages[remote_i]->version)) {
            struct pkg_update pkgupdt = {
                .name = currpkg_l->name,
                .version_local = currpkg_l->version,
                .version_remote = database.packages[remote_i]->version
            };
            updatepkg.current = pkgupdt;
            updatepkg.next->prev = &updatepkg;
            updatepkg = *(updatepkg.next);
            updatec++;
        }
    }
    
    if (!updatec) {
        printf("No packages need to be updated.\n");
        return 0;
    }
    
    /* this is a lazy fix, but i'm too tired to do it properly rn */
    updatepkg = *(updatepkg.prev);
    updatepkg.next = NULL;
    while (updatepkg.prev) {
        updatepkg = *(updatepkg.prev);
    }
    
    printf("The following packages can be updated:\n");
    
    while (updatepkg.next != NULL) {
        printf("%s (%s -> %s) ", updatepkg.current.name, updatepkg.current.version_local, updatepkg.current.version_remote);
        updatepkg = *(updatepkg.next);
    }
    
    // seek back before passing to upgrade();
    while (updatepkg.prev) {
        updatepkg = *(updatepkg.prev);
    }
    
    printf("\nDo you wish to install these updates now? [Y/n] ");
    char response = getchar();
    if (response == 'n' || response == 'N')
        return 0;
    if (response == 'y' || response == 'Y')
        return upgrade(updatepkg);
    return 0;
}

int upgrade(struct ll_node_update updpkglst) {
    while (updpkglst.next != NULL) {
        updpkglst = *(updpkglst.next);
    }
    return 0;
}
