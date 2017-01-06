#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "rk-include.h"

#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"

// jump add_jump
void add_jump(FILE *src, char *name)
{
    /* shorten, since it's used often */
    register int i = jump_count;

    /* remove dash */
    strsnip(name, strlen(name)-1);

    /* pre-existing jump */
    if (existing_jump(name) != -1)
        return;

    /* set name */
    jumps[i].name = malloc(MAXWORD);
    memset(jumps[i].name, 0, MAXWORD);
    memcpy(jumps[i].name, name, strlen(name));

    /* set file position */
    long offset     = ftell(src);
    jumps[i].offset = offset;

    /* increment count, woo #uselesscomments */
    jump_count++;
}

// jump jump  * oh crap *
/* I will be using goto statements in this function
   that implements gotos. Deal with it. */
void jump(FILE *src, char *jump_name)
{
    int i;

    for (i = 0; i < jump_count; i++)
        if (!strcmp(jumps[i].name, jump_name))
            goto end;

end:
    fseek(src, jumps[i].offset, SEEK_SET);
}

// jump existing_jump
int existing_jump(char *name)
{
    int i;

    for (i = 0; i < jump_count; i++)
        if (!strcmp(jumps[i].name, name))
            return i;

    return -1;
}
