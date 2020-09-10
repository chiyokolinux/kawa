#include "depresolve.h"

void resolve_recursive(struct pkglist *nodelist, struct pkg_update *updatepkgs[], char *current, struct pkglist *database, struct pkglist *installed, int depth, int *updatec, int force_install) {
    if (depth >= MAXDEPTH) {
        printf("Error: maximum recursion depth of %d reached during dependency resolving.\n", MAXDEPTH);
        printf("This is most commonly caused by cyclic dependencies, please contact the maintainer of the package you want to install.\n");
        exit(-2);
    }
    
    if (current[0] == '-')
        return;
    
    struct package *currpkg;
    for (int i = 0; i < database->pkg_count; i++) {
        currpkg = database->packages[i];
        if (!strcmp(currpkg->name, current)) {
            int current_installed = 0;
            if (!force_install) {
                for (int i2 = 0; i2 < installed->pkg_count; i2++) {
                    if (!strcmp(current, installed->packages[i2]->name)) {
                        current_installed = 1;
                        break;
                    }
                }
            }
            
            if (current_installed) {
                struct pkg_update *pkgupdt = pkg_has_update(current, database, installed);
                if (pkgupdt != NULL) {
                    updatepkgs[*updatec] = pkgupdt;
                    *updatec = *updatec + 1;
                }
            }
            
            if (!currpkg->depends.retval[0][0] == '\0') {
                for (int i = 0; i < currpkg->depends.retc; i++) {
                    int in_queue = 0;
                    for (int i3 = 0; i3 < nodelist->pkg_count; i3++) {
                        if (!strcmp(currpkg->depends.retval[i], nodelist->packages[i3]->name)) {
                            in_queue = 1;
                            break;
                        }
                    }
                    
                    if (!in_queue)
                        resolve_recursive(nodelist, updatepkgs, currpkg->depends.retval[i], database, installed, depth + 1, updatec, force_install);
                }
            }
            
            if (!current_installed) {
                nodelist->packages[nodelist->pkg_count] = currpkg;
                nodelist->pkg_count++;
            }
            return;
        }
    }
    
    printf("Error: Package %s not found, but required by another package.\n", current);
    printf("This error may be solved by kawa sync, but if not, please contact the maintainer of the package you want to install.\n");
    exit(-3);
}
