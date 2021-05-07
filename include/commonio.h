#ifndef _COMMON_IO_H
#define _COMMON_IO_H

#pragma once

#include <stdio.h>
#include <unistd.h>

#include "error_utils.h"

int f_skipline(FILE *fstream);
int f_readline(FILE *fstream, char **str);
int f_readword(FILE *fstream, char **str);


#endif