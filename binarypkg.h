#ifndef BINARYPKG_H
#define BINARYPKG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "config.h"
#include "datatypes.h"
#include "kawafile.h"

int binarypkg_gen_kawafile(char pkgname[], char filetype[]);
int binarypkg_install(char pkgname[], char filetype[]);
int binarypkg_remove(char pkgname[]);
int binarypkg_update(char pkgname[], char filetype[]);

#endif // BINARYPKG_H
