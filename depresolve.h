#ifndef DEPRESOLVE_H
#define DEPRESOLVE_H

#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "database.h"
#include "datatypes.h"

void resolve_recursive(struct pkglist *nodelist, char *current, struct pkglist *database, struct pkglist *installed, int depth);

#endif // DEPRESOLVE_H
