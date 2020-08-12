enum pkgtype{source, patch, meta, binary};
enum license{GPLv1, GPLv2, GPLv3, LGPLv1, LGPLv2, LGPLv21, LGPLv3, BSD, FREEOTHER, OTHER, CCBYSA, ZLIB, PUBLICDOMAIN, OSLv3, MIT, OPENSSL, PSFL, APACHE, APACHEv2};
enum scripts{preinstall, preupdate, dobuild, doinstall, postinstall, postupdate};

struct package {
    char name[256];
    char description[2048];
    char version[128];
    char archiveurl[2048];
    char maintainer[256];
    char depends[][256];
    char conflicts[][256];
    char configurecmd[512];
    char configureopts[][2048];
    enum pkgtype type;
    char sepbuild;
    char uninstallcmd[512];
    enum license licenseobj;
    enum scripts scriptsobj[6];
}

struct pkglist {
    int pkg_count;
    struct package packages[];
} 

struct ll_node {
    struct package current;
    struct ll_node *next;
    struct ll_node *prev;
}
