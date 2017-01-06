#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "rk-include.h"

#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"

int breaklevel = 0;

// jump wait_for_character
void wait_for_character(FILE *f, int delim, int print)
{
    static int escape = 0;
    int c;

    while (((c = getc(f)) != delim) && (c != EOF))
        if (print)
            rkputchar(c);
}

// this is kinda bad but I don't know how to improve it.
// jump break_from
void break_from(FILE *f, char *open, char *close)
{
#ifdef WHILE_DEBUG
    printf("Breaking... ");
#endif

    char *buf = malloc(MAXWORD);
    int recursive = 1;

    getword(f, buf, " \"\n");

#ifdef WHILE_DEBUG
    printf("\"%s\" %d\n", buf, breaklevel);
#endif

    if (!strcmp(buf, open))
        breaklevel++;
    if (!strcmp(buf, close))
        breaklevel--;

    if (breaklevel == -1) {
        breaklevel++;
        recursive = 0;
    }

    free(buf);

    if (recursive)
        break_from(f, open, close);
#ifdef WHILE_DEBUG
    else
        sleep(1);
#endif
}
