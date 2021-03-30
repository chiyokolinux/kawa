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

#include <string.h>
#include <errno.h>

#include "config.h"

#include "help.h"
#include "install.h"
#include "makepackages.h"
#include "remove.h"
#include "repoadd.h"
#include "update.h"
#include "search.h"
#include "show.h"
#include "cacheclean.h"

int main(int argc, char *argv[]) {
    // if no args given, exit with help
    if (argc == 1)
        return help();
    // if args given, send to function
    else if ((!strcmp(argv[1], "sync") && argc == 2) || (!strcmp(argv[1], "refresh") && argc == 2))
        return sync_all();
    else if ((!strcmp(argv[1], "sync") && argc == 3) || (!strcmp(argv[1], "refresh") && argc == 3))
        return sync_repo_cli(argv[2]);
    else if (!strcmp(argv[1], "install") && argc >= 3)
        return install(argc, argv);
    else if (!strcmp(argv[1], "update") && argc == 2)
        return update();
    else if (!strcmp(argv[1], "remove") && argc >= 3)
        return pkg_remove(argc, argv);
    else if (!strcmp(argv[1], "search") && argc == 3)
        return search(argv[2]);
    else if (!strcmp(argv[1], "show") && argc == 3)
        return show(argv[2]);
    else if (!strcmp(argv[1], "help") && argc == 2)
        return help();
    else if (!strcmp(argv[1], "version") && argc == 2)
        return version();
    else if (!strcmp(argv[1], "addrepo") && argc == 4)
        return repoadd(argv[2], argv[3]);
    else if (!strcmp(argv[1], "makepackages") && argc == 2)
        return makepackages();
    else if (!strcmp(argv[1], "cacheclean") && argc == 2)
        return cacheclean();
    // if no args lengths matched, just exit with help and non-zero exit code.
    return help() + 1;
}
