/**
 * this file is part of kawa
 * Copyright (c) 2020-2021 Emily <elishikawa@jagudev.net>
 *
 * kawa is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * kawa is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kawa.  If not, see <https://www.gnu.org/licenses/>.
**/

#ifndef KAWAFILE_H
#define KAWAFILE_H

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include "config.h"
#include "datatypes.h"

// both of these functions are blocking, because we want to keep the installation in sync
// things might break if we don't (and we don't want anything breaking, do we?)
int spawnwait(char *const argv[]);
int kawafile_run(char pkgname[], char opreation[]);
void kawafile_dir_create(char pkgname[]);
void kawafile_dir_remove(struct package pkg[]);

#endif // KAWAFILE_H
