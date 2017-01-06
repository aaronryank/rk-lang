#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "rk-include.h"

#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"

// jump add_goto
void add_goto(FILE *src, char *name)
{
    /* shorten, since it's used often */
    register int i = goto_count;

    /* remove dash */
    strsnip(name, strlen(name)-1);

    if (existing_goto(name) != -1)
        return;

    /* set name */
    gotos[i].name = malloc(MAXWORD);
    memset(gotos[i].name, 0, MAXWORD);
    memcpy(gotos[i].name, name, strlen(name));

    /* set file position */
    long offset     = ftell(src);
    gotos[i].offset = offset;

    /* increment count, woo #uselesscomments */
    goto_count++;
}

// jump jump  * oh crap *
/* I will be using goto statements in this function
   that implements gotos. Deal with it. */
void jump(FILE *src, char *goto_name)
{
    int i;

    for (i = 0; i < goto_count; i++)
        if (!strcmp(gotos[i].name, goto_name))
            goto end;

end:
    fseek(src, gotos[i].offset, SEEK_SET);
}

// jump existing_goto
int existing_goto(char *name)
{
    int i;

    for (i = 0; i < goto_count; i++)
        if (!strcmp(gotos[i].name, name))
            return i;

    return -1;
}
