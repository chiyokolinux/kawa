#ifndef METAPKG_H
#define METAPKG_H

#include <stdio.h>

#include "config.h"
#include "datatypes.h"
#include "kawafile.h"

int metapkg_gen_kawafile(char pkgname[], struct package *pkgobj);
int metapkg_install(char pkgname[], struct package *pkgobj);
int metapkg_remove(char pkgname[]);

#endif // METAPKG_H
