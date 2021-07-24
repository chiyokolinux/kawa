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

#ifndef INSTALL_H
#define INSTALL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/stat.h>

#include "config.h"
#include "datatypes.h"
#include "database.h"
#include "depresolve.h"
#include "kawafile.h"
#include "reposync.h"
#include "error.h"

#include "metapkg.h"
#include "binarypkg.h"
#include "sourcepkg.h"

int install(int pkgc, char *pkgnames[]);
int download_install_packages(struct pkglist *nodelist, struct pkg_update **updatepkgs, int *updatec, struct pkglist *database, struct pkglist *installed, struct strarr_retval pkg_deptypes, int pkgc, char *pkgnames[]);
int install_no_deps(struct package *currpkg, struct pkglist *database, struct pkglist *installed, int manual_installed, int is_update);
int download_package(struct package *currpkg, struct pkglist *database, int is_update);
int download_archive(struct package *dlpackage, int force);
int download_scripts(struct package *dlpackage, char *baseurl);
char *str_replace(char *orig, char *rep, char *with);
int add_db_entry(struct package *package, int manual_installed, struct pkglist *database, struct pkglist *installed);
char *whitespace_join(struct strarr_retval to_join);

#endif // INSTALL_H
