#include <stdio.h>
#include <stdarg.h>
#include "rk-include.h"

void error(int die, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, "Error %d: ", ++error_count);
    vfprintf(stderr, fmt, args);

    va_end(args);

    if (die)
        clean_exit(-1);
}
