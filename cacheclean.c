#include "cacheclean.h"

int cacheclean() {
    // init base path
    char path[strlen(INSTALLPREFIX)+23];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/kawafiles/");

    // sprintf path variable into find & delete command
    char command[strlen(path)+45];
    sprintf(command, "find %s -name package.src.kawapkg -type f -delete", path);
    
    // execute find & delete command
    int retval = system(command);
    return retval;
}
