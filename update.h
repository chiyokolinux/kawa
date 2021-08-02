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

#ifndef UPDATE_H
#define UPDATE_H

#include <stdio.h>
#include <string.h>

#include "reposync.h"
#include "database.h"
#include "datatypes.h"
#include "install.h"
#include "depresolve.h"
#include "error.h"

int update();
int upgrade(struct pkg_update *updpkglst[], int updatec, struct pkglist *database, struct pkglist *installed, struct pkglist *nodelist, struct strarr_retval pkg_deptypes);
struct pkg_update *pkg_has_update(char pkgname[], struct pkglist *database, struct pkglist *installed);

#endif // UPDATE_H
