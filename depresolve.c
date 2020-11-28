#include "depresolve.h"

/**
 * recursive DFS implementation
 * resolves dependencies of one package into
 * a nodelist.
 * the nodelist is order-sorted (first package first)
 * it cannot handle dependency circles, that is up to
 * the packager to handle.
**/
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
            check_package_source(currpkg, i, database, installed, current_installed);
            
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

/**
 * this function looks up which repository should be used
 * as the source of a package.
 * if the user has already once selected a source for the package
 * (e.g. during installation), this source is used again.
 * otherwise, the user is prompted to select one manually.
**/
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
        if (strcmp(database->repos->repos[*currpkg->repoindex]->reponame, installed->packages[installed_i]->maintainer)) {
            // go back to first package with searched name
            while (!strcmp(currpkg->name, database->packages[database_i - 1]->name))
                database_i--;

            while (strcmp(database->repos->repos[*currpkg->repoindex]->reponame, installed->packages[installed_i]->maintainer))
                database_i++;

            if (strcmp(currpkg->name, database->packages[database_i]->name)) {
                fprintf(stderr, "\nPackage %s not found in preassigned repository %s\n", currpkg->name, installed->packages[installed_i]->maintainer);
                exit(-4);
            }

            currpkg = database->packages[database_i];
            return;
        }
    } else {
        // go back to first package with searched name
        while (!strcmp(currpkg->name, database->packages[database_i - 1]->name))
            database_i--;

        printf("\nPackage %s has been found in multiple repositories.\n", currpkg->name);
        printf("Please choose one of the following repositories:\n");
        
        printf(" * [1] %s\n", database->repos->repos[*database->packages[database_i]->repoindex]->reponame);

        int repoidx = 2;
        while (strcmp(currpkg->name, database->packages[database_i]->name)) {
            printf("   [%d] %s\n", repoidx, database->repos->repos[*database->packages[database_i]->repoindex]->reponame);
            repoidx++;
            database_i++;
        }

        printf("Select one: ");
        fflush(stdout);
        scanf("%d", &repoidx); // TODO: allow default value in a non-stupid way
        printf("\n");
        // we allow values over the max, if the user is stupid enough to enter an invalid number
        // it's really their own fault i guess
        // acutally this is just a bad excuse to not allocate one more integer

        // go back to first package with searched name
        while (!strcmp(currpkg->name, database->packages[database_i - 1]->name))
            database_i--;

        for (int i = 1; i < repoidx; i++)
            database_i++;
        
        currpkg = database->packages[database_i];
        return;
    }
}

/**
 * returns the highest set bit of a 32bit int
**/
int hibit_xor(unsigned int n) {
    n |= (n >>  1);
    n |= (n >>  2);
    n |= (n >>  4);
    n |= (n >>  8);
    n |= (n >> 16);
    return n ^ (n >> 1);
}

/**
 * simple linear bsort implementation for struct package's
 * also returns where the match was found
 * this addons is not present in any standard libc bsort,
 * that's why I wrote this
**/
struct package *bsearch_pkg(char pkgname[], struct pkglist *database, int *i) {
    *i = database->pkg_count / 2;
    int modifier = hibit_xor(*i);
    int cmpres = 0;
    while ((cmpres = strcmp(database->packages[*i]->name, pkgname))) {
        modifier /= 2;
        if (cmpres < 0) {
            *i += modifier;
        } else {
            *i -= modifier;
        }
        if (modifier == 0) {
            fprintf(stderr, "Error: Package %s not found (try kawa sync)\n", pkgname);
            exit(1);
        }
    }
    return database->packages[*i];
}
