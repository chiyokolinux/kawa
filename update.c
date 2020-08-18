#include "update.h"

int update() {
    sync_all();
    struct pkglist *database = get_all_packages();
    struct pkglist *installed = get_installed_packages();
    struct pkg_update *updatepkg[installed->pkg_count];
    int remote_i = 0;
    int updatec = 0;
    for (int i = 0; i < installed->pkg_count; i++) {
        struct package *currpkg_l = installed->packages[i];
        while (strcmp(currpkg_l->name, database->packages[remote_i]->name))
            remote_i++; // this should work because the package lists are sorted (hopefully)
        if (strcmp(currpkg_l->version, database->packages[remote_i]->version)) {
            struct pkg_update *pkgupdt = malloc(sizeof(struct pkg_update));
            *pkgupdt = (struct pkg_update) {
                .name = currpkg_l->name,
                .version_local = currpkg_l->version,
                .version_remote = database->packages[remote_i]->version
            };
            updatepkg[updatec++] = pkgupdt;
        }
    }
    
    if (!updatec) {
        printf("No packages need to be updated.\n");
        return 0;
    }
    
    printf("The following packages can be updated:\n");
    
    for (int i = 0; i < updatec; i++) {
        printf("%s (%s -> %s) ", updatepkg[i]->name, updatepkg[i]->version_local, updatepkg[i]->version_remote);
    }
    
    printf("\nDo you wish to install these updates now? [Y/n] ");
    char response = getchar();
    if (response == 'n' || response == 'N')
        return 0;
    if (response == 'y' || response == 'Y' || response == '\n')
        return upgrade(updatepkg);
    return 1;
}

int upgrade(struct pkg_update *updpkglst[]) {
    printf("upgrading! %s", updpkglst[0]->name);
    return 0;
}
