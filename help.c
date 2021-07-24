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

#include "help.h"

int help() {
    printf("kawa - help\n");
    printf("-----------\n\n");
    printf("basic commands:\n");
    printf("    kawa in|install [package(s)]\n");
    printf("    kawa rm|remove [package(s)]\n");
    printf("    kawa up|update\n");
    printf("    kawa se|search [term]\n");
    printf("    kawa sh|show [package]\n");
    printf("    kawa sy|sync <reponame>\n");
    printf("    kawa rf|refresh <reponame> (alias for sync)\n");
    printf("    kawa cc|cacheclean\n");
    printf("    kawa he|help\n");
    printf("    kawa ve|version\n");
    printf("    kawa mp|makepackages\n");
    printf("    kawa ar|addrepo [name] [url]\n");
    printf("\ncommand options:\n");
    printf("    install:\n");
    printf("        -D, --no-depends      : Do not resolve dependencies and install\n");
    printf("                                only the specified packages.\n");
    printf("        -S, --sync            : Sync all package repositories before resolving the\n");
    printf("                                dependencies and installing the packages.\n");
    printf("        -r, --reinstall       : Install the package and all of its dependencies,\n");
    printf("            --force             even if it is already installed and no updates\n");
    printf("                                for any of said packages are available.\n");
    printf("        -I, --ignore-updates  : Ignore packages that the to-be-installed package\n");
    printf("                                depends on that have available updates.\n");
    printf("        -A, --accept-nonfree  : Do not warn before installing non-free packages.\n");
    printf("                                Use this flag with caution.\n");
    return 0;
}

int version() {
    printf("kawa %s\n", VERSION);
    return 0;
}
