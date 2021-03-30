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
