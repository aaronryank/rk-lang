#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "rk-include.h"

#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"

char escape_code[100];

// jump rkread
void rkread(int idx)
{
    char *buf;
    int c, i;
    int type = var_list[idx].type;

    if (type == CHARACTER) {
        var_list[idx].value = (void *) getchar();
        return;
    }

    buf = malloc(MAXWORD);
    memset(buf, 0, MAXWORD);

    getword(stdin, buf, "\n");

    if (type == BOOLEAN) {
        if (!strcmp(buf,"")) { // common character fallthrough issue
            rkread(idx);
            return;
        }

        if (!strcmp(buf, "true"))
            var_list[idx].value = (void *) 1;
        else if (!strcmp(buf, "false"))
            var_list[idx].value = (void *) 0;
        else
            var_list[idx].value = ((void *) (buf[0] - '0'));
    }
    else if (type == INTEGER) {
        var_list[idx].value = (void *) atoi(buf);
    }
    else {
        var_list[idx].value = malloc(MAXWORD);
        memcpy(var_list[idx].value, buf, strlen(buf));
    }

    free(buf);
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
void rkprint(int idx)
{
    int i = 0;

    char *str = malloc(MAXWORD);
    memset(str, 0, MAXWORD);

    /* set string */
    if (var_list[idx].type == INTEGER) {
        itoa((int) var_list[idx].value, str);
    }
    else if (var_list[idx].type == CHARACTER) {
        str[0] = (char) var_list[idx].value;
    }
    else if (var_list[idx].type == BOOLEAN) {
        if ((int) var_list[idx].value == 0)
            memcpy(str, "false", 5);
        else
            memcpy(str, "true", 4);
    }
    else {
        memcpy(str, var_list[idx].value, strlen((char *) var_list[idx].value));
    }

    /* print the string */
    while (str[i])
        rkputchar(str[i++]);

    /* helpful for integers */
    if ((var_list[idx].type == INTEGER) || (var_list[idx].type == BOOLEAN))
        putchar(' ');

    /* yup */
    free(str);
}

// jump rkputchar
void rkputchar(char c)
{
    static volatile int escape = 0;
    static volatile int ecode = 0;
    static volatile int e_idx = 0;

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
