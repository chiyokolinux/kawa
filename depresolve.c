#include "depresolve.h"

void resolve_recursive(struct pkglist *nodelist, struct pkg_update *updatepkgs[], char *current, struct pkglist *database, struct pkglist *installed, int depth, int *updatec, int force_install, int ignore_updates) {
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
            
            if (current_installed && !ignore_updates) {
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
                        resolve_recursive(nodelist, updatepkgs, currpkg->depends.retval[i], database, installed, depth + 1, updatec, force_install, ignore_updates);
                }
            }
            
            if (!current_installed) {
                nodelist->packages[nodelist->pkg_count] = currpkg;
                nodelist->pkg_count++;
            }
            return;
        }
    }
    
    fprintf(stderr, "Error: Package %s not found, but required by another package.\n", current);
    fprintf(stderr, "This error may be solved by kawa sync, but if not, please contact the maintainer of the package you want to install.\n");
    exit(-3);
}

void check_package_source(struct package *currpkg, int database_i, struct pkglist *database, struct pkglist *installed, int is_installed) {
    // if there are no packages with the same name (remember: pkglists are always sorted!), just leave currpkg alone
    if (strcmp(currpkg->name, database->packages[database_i + 1]->name) && strcmp(currpkg->name, database->packages[database_i - 1]->name))
        return;

    // if the installed status hasn't already been calculated, do it now
    int installed_i = -1;
    if (is_installed == -1) {
        for (int i2 = 0; i2 < installed->pkg_count; i2++) {
            if (!strcmp(currpkg->name, installed->packages[i2]->name)) {
                is_installed = 1;
                installed_i = i2;
                break;
            }
        }
        if (is_installed == -1)
            is_installed = 0;
    }

    if (is_installed) {
        if (strcmp(database->repos->repos[*currpkg->repoindex]->reponame, installed_i->packages[installed_i]->maintainer)) {
            // go back to first package with searched name
            while (!strcmp(currpkg->name, database->packages[database_i - 1]->name))
                database_i--;

            while (strcmp(database->repos->repos[*currpkg->repoindex]->reponame, installed_i->packages[installed_i]->maintainer))
                database_i++;

            if (strcmp(currpkg->name, database->packages[i]->name)) {
                fprintf(stderr, "Package %s not found in preassigned repository %s\n", currpkg->name, installed_i->packages[installed_i]->maintainer);
                exit(-4);
            }

            currpkg = database->packages[database_i];
            return;
        }
    }
}
