#ifndef UPDATE_H
#define UPDATE_H

#include <stdio.h>
#include <string.h>

#include "reposync.h"
#include "database.h"
#include "datatypes.h"

int update();
int upgrade(struct pkg_update *updpkglst[]);
struct pkg_update *pkg_has_update(char pkgname[], struct pkglist *database, struct pkglist *installed);

#endif // UPDATE_H
