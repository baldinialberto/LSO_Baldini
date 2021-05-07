#ifndef _COMMON_IO_H
#define _COMMON_IO_H

#pragma once

#include <stdio.h>
#include <unistd.h>

#include "error_utils.h"

size_t f_skipline(FILE *fstream);
size_t f_readline(FILE *fstream, char **str);
size_t f_readword(FILE *fstream, char **str);


#endif