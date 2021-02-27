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
int install_no_deps(struct package *currpkg, struct pkglist *database, int manual_installed, int is_update);
int download_package(struct package *currpkg, struct pkglist *database, int is_update);
int download_archive(struct package *dlpackage, int force);
int download_scripts(struct package *dlpackage, char *baseurl);
char *str_replace(char *orig, char *rep, char *with);
int add_db_entry(struct package *package, int manual_installed, struct pkglist *database);
char *whitespace_join(struct strarr_retval to_join);

#endif // INSTALL_H
