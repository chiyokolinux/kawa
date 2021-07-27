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

#include "kawafile.h"

int spawnwait(char *const argv[]) {
    int status;
    pid_t chpid = fork();
    switch (chpid) {
        case 0:
            execvp(argv[0], argv);
            perror("execvp");
            _exit(1);
        case -1:
            perror("fork");
            return -1;
        default:
            waitpid(chpid, &status, 0);
            return WIFSIGNALED(status) ? WTERMSIG(status) + 128 : WEXITSTATUS(status);
    }
}

int kawafile_run(char pkgname[], char operation[]) {
    // argv = INSTALLPREFIX + /etc/kawa.d/kawafiles/${pkgname}/Kawafile, ${operation}, NULL
    // Kawafiles only need script path and operation and do the rest by themselves, so there's no need for
    // complicated array prepending, NULL appending, type conversion or other stuff
    // it sounds smart right now, but we'll see tomorrow
    char path[strlen(INSTALLPREFIX)+32+strlen(pkgname)];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/kawafiles/");
    strcat(path, pkgname);
    strcat(path, "/Kawafile");
    return spawnwait((char *const[]){ path, operation, NULL });
}

void kawafile_dir_create(char pkgname[]) {
    struct stat st = {0};
    
    char path[strlen(INSTALLPREFIX)+23+strlen(pkgname)];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/kawafiles/");
    strcat(path, pkgname);
    
    if (stat(path, &st) == -1) {
        mkdir(path, 0700);
    }
}

void kawafile_dir_remove(struct package pkg[]) {
    struct stat st = {0};

    char path[strlen(INSTALLPREFIX)+23+strlen(pkg->name)];
    strcpy(path, "");
    strcat(path, INSTALLPREFIX);
    strcat(path, "/etc/kawa.d/kawafiles/");
    strcat(path, pkg->name);

    if (stat(path, &st) == -1) {
        return;
    }

    char path_appendable[strlen(INSTALLPREFIX)+72+strlen(pkg->name)];
    strcpy(path_appendable, path);
    strcat(path_appendable, "/");
    int null_byte_index = strlen(path_appendable);

    // delete all script files
    for (int i = 0; i < pkg->scripts.retc; i++) {
        strcat(path_appendable, pkg->scripts.retval[i]);

        unlink(path_appendable);

        path_appendable[null_byte_index] = '\0';
    }

    // delete kawafile
    strcat(path_appendable, "Kawafile");
    unlink(path_appendable);

    // delete cached package file
    path_appendable[null_byte_index] = '\0';
    strcat(path_appendable, "package.src.kawapkg");
    unlink(path_appendable);

    // remove the parent directory which contained all the files
    rmdir(path);
}
