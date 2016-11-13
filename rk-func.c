#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "rk-include.h"

#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"

char escape_code[100];

// jump rkread
int rkread(int type)
{
    char buf[MAXWORD] = {0};
    int c, i;

    for (i = 0; ((c = getchar()) != EOF) && (c != ' ') && (c != '\n'); i++)
        buf[i] = c;

    buf[i+1] = '\0';

    if (type == BOOLEAN) {
        if (!strcmp(buf, "true"))
            return 1;
        else if (!strcmp(buf, "false"))
            return 0;
        else
            return buf[0] - '0';
    } else if (type == INTEGER) {
        return atoi(buf);
    }

    return 0;
}

// jump remove_int
/* remove count integers from arr, starting at index idx */
void remove_int(int (*arr)[], int idx, int count)
{
    int i, j;

    for (i = 0; i < count; i++)
        for (j = idx; (*arr)[j]; j++)
            (*arr)[j] = (*arr)[j+1];
}

// jump rkprint
void rkprint(char *str)
{
    int i = 0;

    while (str[i]) {
        rkputchar(str[i]);
        i++;
    }
}

// jump rkputchar
void rkputchar(char c)
{
    static int escape = 0;
    static int ecode = 0;
    static int e_idx = 0;

    if ((c == '^') && (escape == 0)) {
        escape = 1;
    } else if (escape == 1) {
        switch (c) {
          case 'n': putchar('\n'); break;
          case 'b': putchar('\b'); break;
          case 't': putchar('\t'); break;
          case 'v': putchar('\v'); break;
          case 'r': putchar('\r'); break;
          case 'f': putchar('\f'); break;
          case 'a': putchar('\a'); break;
          case 'e': ecode = 1;
                    memset(escape_code, 0, 100);
                    break;
          default:  putchar(c); 
        }
        escape = 0;
    } else if (ecode == 1) {
        escape_code[e_idx++] = c;

        if (isalpha(c)) {
            printf("\e%s", escape_code);
            ecode = e_idx = 0;
        }
    } else {
        putchar(c);
    }
}
