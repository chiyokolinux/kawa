#ifndef DEPRESOLVE_H
#define DEPRESOLVE_H

#include <stdio.h>
#include <stdlib.h>

#include "database.h"
#include "datatypes.h"

struct dependency_graph *resolve_recursive(struct dependency_graph *nodelist, char *current, struct pkglist *database, struct pkglist *installed);

#endif // DEPRESOLVE_H
