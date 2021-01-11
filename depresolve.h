#ifndef DEPRESOLVE_H
#define DEPRESOLVE_H

#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "update.h"
#include "database.h"
#include "datatypes.h"

void resolve_recursive(struct pkglist *nodelist, struct pkg_update *updatepkgs[], char *current, struct pkglist *database, struct pkglist *installed, int depth, int *updatec, int force_install, int ignore_updates, unsigned int *dependency_types);
void check_package_source(struct package *currpkg, int database_i, struct pkglist *database, struct pkglist *installed, int is_installed);
int hibit_xor(unsigned int n);
struct package *bsearch_pkg(char pkgname[], struct pkglist *database, int *i,  int pkg_is_dependency);

#endif // DEPRESOLVE_H
