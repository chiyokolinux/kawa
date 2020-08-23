#ifndef DATATYPES_H
#define DATATYPES_H

enum pkgtype{source, patch, meta, binary};
enum license{GPLv1, GPLv2, GPLv3, LGPLv1, LGPLv2, LGPLv21, LGPLv3, BSD, FREEOTHER, OTHER, CCBYSA, ZLIB, PUBLICDOMAIN, OSLv3, MIT, OPENSSL, PSFL, APACHE, APACHEv2, VIM};
enum scripts{preinstall, preupdate, dobuild, doinstall, postinstall, postupdate};

struct strarr_retval {
    int retc;
    char **retval;
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
};

struct pkglist {
    int pkg_count;
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

struct repository {
    char reponame[127];
    char repourl[511];
};

struct repolist {
    int repo_count;
    struct repository repos[];
};

#endif // DATATYPES_H
