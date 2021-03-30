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

#ifndef DATATYPES_H
#define DATATYPES_H

enum pkgtype{source, patch, meta, binary};
enum license{GPLv1, GPLv2, GPLv3, LGPLv1, LGPLv2, LGPLv21, LGPLv3, BSD, FREEOTHER, OTHER, CCBYSA, ZLIB, PUBLICDOMAIN, OSLv3, MIT, OPENSSL, PSFL, APACHE, APACHEv2, VIM};
enum scripts{preinstall, preupdate, dobuild, doinstall, postinstall, postupdate};

struct strarr_retval {
    int retc;
    char **retval;
};

struct repository {
    char reponame[127];
    char repourl[511];
    char baseurl[511];
};

struct repolist {
    int repo_count;
    struct repository **repos;
};

struct package {
    char *name;
    char *description;
    char *version;
    char *archiveurl;
    char *maintainer;
    struct strarr_retval depends;
    struct strarr_retval conflicts;
    char *configurecmd;
    struct strarr_retval configureopts;
    char *type;
    char *sepbuild;
    char *uninstallcmd;
    char *license;
    struct strarr_retval scripts;
    int *repoindex;
};

struct pkglist {
    int pkg_count;
    struct repolist *repos;
    struct package **packages;
};

struct ll_node {
    struct package current;
    struct ll_node *next;
    struct ll_node *prev;
};

struct pkg_update {
    char *name;
    char *version_local;
    char *version_remote;
};

struct ll_node_update {
    struct pkg_update current;
    struct ll_node_update *next;
    struct ll_node_update *prev;
};

#endif // DATATYPES_H
