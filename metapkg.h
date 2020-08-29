#ifndef METAPKG_H
#define METAPKG_H

#include <stdio.h>

#include "config.h"
#include "datatypes.h"
#include "kawafile.h"

int metapkg_gen_kawafile(char pkgname[]);
int metapkg_install(char pkgname[]);
int metapkg_remove(struct package *package);
int metapkg_update(char pkgname[]);

#endif // METAPKG_H
