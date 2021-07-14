#ifndef PRINT_UTILS_H
#define PRINT_UTILS_H

#include <stdio.h>

#define pu_print_flush(stream, to_print) \
fprintf(stream, to_print);               \
fflush(stream);

#endif
