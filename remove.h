#ifndef REMOVE_H
#define REMOVE_H

#include <stdio.h>

#include "database.h"
#include "kawafile.h"
#include "binarypkg.h"
#include "metapkg.h"
#include "sourcepkg.h"

int pkg_remove(int pkgc, char *pkgnames[]);
int remove_single(char pkgname[], struct pkglist *installed, struct pkglist *database);
void remove_db_entry(struct package *package, struct pkglist *installed);

#endif // REMOVE_H
