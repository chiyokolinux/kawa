#ifndef KAWAFILE_H
#define KAWAFILE_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// both of these functions are blocking, because we want to keep the installation in sync
// things might break if we don't (and we don't want anything breaking, do we?)
void spawnwait(char *const argv[]);
void kawafile_run(char pkgname[], char opreation[]);
void kawafile_dir_create(char pkgname[]);

#endif // KAWAFILE_H
