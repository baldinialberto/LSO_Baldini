#ifndef PRINT_UTILS_H
#define PRINT_UTILS_H

#include <stdio.h>

#define pu_print_flush(stream, ...) \
fprintf(stream, __VA_ARGS__);       \
fflush(stream);

#define pu_print_pos(stream, message) \
pu_print_flush(stream, "%s at function {%s} line [%d] in file {%s}\n", \
message, __func__, __LINE__, __FILE__);


#endif
