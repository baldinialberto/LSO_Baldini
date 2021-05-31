#ifndef _DIR_UTILS_H
#define _DIR_UTILS_H

#pragma once

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>

#include "string_utils.h"
#include "list_utils.h"
#include "mem_utils.h"

int du_getfilepaths_from_dir(const char *dirpath, int nfilepaths, u_list *filelist, u_list *dirlist);
char du_filenodetype(struct stat *_stat);

#endif