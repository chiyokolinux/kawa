#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct pkglist get_packages_from_repo(char reponame[]);
struct pkglist get_all_packages();
int compare_strings(const void* a, const void* b);
struct pkglist sort_package_list(struct pkglist orig_pkglist);
