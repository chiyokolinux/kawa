#include "makepackages.h"

int makepackages() {
    printf("Running mkpackage.sh for every package in packages...  ");
    fflush(stdout);
    return system("sp=\"/-\\|\"; sc=0; spin() { printf \"\\b${sp:sc++:1}\"; ((sc==${#sp})) && sc=0; }; endspin() { printf \"\\r%s\\n\" \"$@\"; }; echo \"Running mkpackage.sh for every package in packages...\" > Kawamake.log; for d in packages/*/; do spin; ${d}/mkpackage.sh >> Kawamake.log; done; endspin;");
}
