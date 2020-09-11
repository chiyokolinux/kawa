#ifndef INSTALL_H
#define INSTALL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/stat.h>

#include "config.h"
#include "datatypes.h"
#include "database.h"
#include "depresolve.h"
#include "kawafile.h"
#include "reposync.h"

#include "metapkg.h"
#include "binarypkg.h"

int install(int pkgc, char *pkgnames[]);
int install_no_deps(char pkgname[], struct pkglist *database, int manual_installed, int is_update);
int download_archive(struct package *dlpackage, char filetype[], int force);
char *str_replace(char *orig, char *rep, char *with);
int add_db_entry(struct package *package, int manual_installed);
char *whitespace_join(struct strarr_retval to_join);

#endif // INSTALL_H
