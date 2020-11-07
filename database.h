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
