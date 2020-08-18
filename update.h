#ifndef UPDATE_H
#define UPDATE_H

#include <stdio.h>
#include <string.h>

#include "reposync.h"
#include "database.h"
#include "datatypes.h"

int update();
int upgrade(struct pkg_update *updpkglst[]);

#endif // UPDATE_H
