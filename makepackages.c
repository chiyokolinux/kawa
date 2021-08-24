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

#include "makepackages.h"

int makepackages() {
    printf("Running mkpackage.sh for every package in packages...  ");
    fflush(stdout);
    return system("sp=\"/-\\|\"; sc=0; "
                  "spin() { printf \"\\b${sp:sc++:1}\"; ((sc==${#sp})) && sc=0; }; "
                  "endspin() { printf \"\\r%s\\n\" \"$@\"; }; "
                  "echo \"Running mkpackage.sh for every package in packages...\" > Kawamake.log; "
                  "for d in packages/*/; do "
                  "spin; ${d}/mkpackage.sh >> Kawamake.log; "
                  "git add ${d}; "
                  "git commit -qm \"bump $(basename ${d}) to v$(sed -ne \"s/^VERSION=\\([a-zA-Z0-9._\\-]\\+\\)$/\\1/p\" ${d}/package.conf)\" > /dev/null 2>&1 && { ./mkdb.py > /dev/null 2>&1 && { git add packages.db && git commit -qm \"rebuild package database\" > /dev/null 2>&1; } || true; } || true; "
                  "done; endspin; "
                  "git add Kawamake.log && git commit -qm \"kawa makepackages\" > /dev/null 2>&1;");
}
