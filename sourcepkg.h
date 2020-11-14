#ifndef SOURCEPKG_H
#define SOURCEPKG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "config.h"
#include "kawafile.h"

int sourcepkg_gen_kawafile(struct package *package, char filetype[]);
int sourcepkg_install(struct package *package, char filetype[]);
int sourcepkg_remove(struct package *package);
int sourcepkg_update(struct package *package, char filetype[]);

#endif // SOURCEPKG_H
