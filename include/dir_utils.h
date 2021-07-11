#ifndef _DIR_UTILS_H
#define _DIR_UTILS_H

#pragma once

#include <sys/stat.h>
#include <list_utils.h>

int du_getfilepaths_from_dir(const char* dirpath, int nfilepaths, u_list* filelist, u_list* dirlist);
char du_filenodetype(struct stat* _stat);

#endif