#ifndef BINARYPKG_H
#define BINARYPKG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "datatypes.h"
#include "kawafile.h"

int binarypkg_gen_kawafile(char pkgname[], struct package *pkgobj);
int binarypkg_install(char pkgname[], struct package *pkgobj);
int binarypkg_remove(char pkgname[]);

#endif // BINARYPKG_H
