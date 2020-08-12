#include "update.h"

int update() {
    sync_all();
    struct pkglist database = get_all_packages();
    for (int i = 0; i < database.pkg_count; i++) {
        struct package currpkg = database.packages[i];
        
    }
}
