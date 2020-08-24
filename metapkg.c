#include "metapkg.h"

int metapkg_gen_kawafile(char pkgname[], struct package *pkgobj) {
    // no patch stuff
    // no install()
    // uninstall: kawa remove ${DEPENDS}.join(" ")
    // but we'll probably do that in the main process
    return 0;
}

int metapkg_install(char pkgname[], struct package *pkgobj) {
    // meta packages only install their depends and remove them on uninstall,
    // so we only need to generate a Kawafile which uninstalls all dependencies
    return metapkg_gen_kawafile(pkgname, pkgobj);
}

int metapkg_remove(char pkgname[]) {
    kawafile_run(pkgname, "remove"); // TODO: change that to for i in depends: uninstall(package)
    return 0;
}
