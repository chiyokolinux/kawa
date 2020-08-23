#ifndef DEPRESOLVE_H
#define DEPRESOLVE_H

#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "update.h"
#include "database.h"
#include "datatypes.h"

void resolve_recursive(struct pkglist *nodelist, struct pkg_update *updatepkgs[], char *current, struct pkglist *database, struct pkglist *installed, int depth, int *updatec);

#endif // DEPRESOLVE_H