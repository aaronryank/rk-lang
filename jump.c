#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "rk-include.h"

#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"

void add_jump(FILE *src, char *name)
{
    int i = jump_count;

    strsnip(name, strlen(name)-1);

    if (existing_jump(name) != -1)
        return;

    jumps[i].name = malloc(MAXWORD);
    memset(jumps[i].name, 0, MAXWORD);
    memcpy(jumps[i].name, name, strlen(name));

    long offset     = ftell(src);
    jumps[i].offset = offset;

    jump_count++;
}

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

int existing_jump(char *name)
{
    int i;

    for (i = 0; i < jump_count; i++)
        if (!strcmp(jumps[i].name, name))
            return i;

    return -1;
}
