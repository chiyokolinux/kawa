#ifndef INSTALL_H
#define INSTALL_H

#include <stdio.h>

#include "config.h"
#include "datatypes.h"
#include "database.h"
#include "depresolve.h"

int install(int pkgc, char *pkgnames[]);
int install_no_deps(char pkgname[]);

#endif // INSTALL_H
