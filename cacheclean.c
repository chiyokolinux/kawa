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

#include "cacheclean.h"

int cacheclean() {
    // init base path
    char path[strlen(INSTALLPREFIX)+23];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/kawafiles/");

    // sprintf path variable into find & delete command
    char command[strlen(path)+48];
    sprintf(command, "find %s -name package.src.kawapkg -type f -delete", path);
    
    // execute find & delete command
    int retval = system(command);
    return retval;
}
