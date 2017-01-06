#include "rk-include.h"
#include <string.h>
#include <stdlib.h>

void read_loop(FILE *src, FILE *dest)
{
    char buf[MAXWORD], *delim;

    memset(buf, 0, sizeof(buf));
    delim = malloc(MAXWORD);

    while (getword(src, buf, " \n"))
        if (!strcmp(buf, "rk:start"))
            break;

/*       --- redesign this loop ---
    while (getword(src, buf, " \n", MAXWORD))
        rk_parse(src, dest, buf);
*/
    int c, ret;

    ret = 0;
    while (!ret) {
        c = getc(src);

        if (c == '"') {
            memcpy(delim, "\"", 1);
        } else {
            memcpy(delim, " \n", 2);
            ungetc(c, src);
        }

        /* if `c == '"'`, will stop at first " read, invalidating the whole operation and missing a character. */
        if (!getword(src, buf, delim)) {
            ret = 1;
        } else {
            if (c == '"') {
                push_char(c, buf);
                strcat(buf, "\"");
            }
            rk_parse(src, dest, buf);
            memset(buf, 0, sizeof(buf));
        }
    }
}
