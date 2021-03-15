#ifndef PATCHPKG_H
#define PATCHPKG_H

#include <stdio.h>

#include "config.h"
#include "datatypes.h"
#include "kawafile.h"

int patchpkg_gen_kawafile(struct package *package);
int patchpkg_install(struct package *package);
int patchpkg_remove(struct package *package);
int patchpkg_update(struct package *package);

#endif // PATCHPKG_H
