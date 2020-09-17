#ifndef REMOVE_H
#define REMOVE_H

#include <stdio.h>

#include "database.h"
#include "kawafile.h"
#include "binarypkg.h"
#include "metapkg.h"

int pkg_remove(int pkgc, char *pkgnames[]);
int remove_single(char pkgname[], struct pkglist *installed);
void remove_db_entry(struct package *package, struct pkglist *installed);

#endif // REMOVE_H
