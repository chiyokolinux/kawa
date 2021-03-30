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

#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "datatypes.h"
#include "config.h"
#include "install.h"
#include "error.h"

struct strarr_retval split_space(char to_split[]);
void parse_csv_line(char line[], struct package* retval, int repoindex);
struct pkglist *get_packages_from_repo(char reponame[], int repoindex);
struct pkglist *get_all_packages();
struct pkglist *get_installed_packages();
int compare_strings(const void* a, const void* b);
struct pkglist *sort_package_list(struct pkglist *orig_pkglist);
void pkglist_free(struct pkglist *packages);
int write_installed_packages(struct pkglist *installed, struct pkglist *database);

#endif // DATABASE_H
