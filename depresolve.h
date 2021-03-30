/**
 * this file is part of kawa
 * Copyright (c) 2020-2021 Emily <elishikawa@jagudev.net>
 *
 * kawa is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * kawa is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kawa.  If not, see <https://www.gnu.org/licenses/>.
**/

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
