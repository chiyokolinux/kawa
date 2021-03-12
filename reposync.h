#ifndef REPOSYNC_H
#define REPOSYNC_H

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>

#include "config.h"
#include "datatypes.h"

int sync_repo(char reponame[], char repourl[]);
int sync_all();
int sync_repo_cli(char reponame_target[]);

#endif
