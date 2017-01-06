#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "rk-include.h"

#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"

// jump set_next_variable
int set_next_variable(char *buf)
{
    if (!strcmp(buf, "var")) {
        next.type = VAR;
        next.var_type = UNTYPED;
    } else if (!strcmp(buf, "int")) {
        next.type = VAR;
        next.var_type = INTEGER;
    } else if (!strcmp(buf, "key")) {
        next.type = VAR;
        next.var_type = CHARACTER;
    } else if (!strcmp(buf, "str")) {
        next.type = VAR;
        next.var_type = STRING;
    } else if (!strcmp(buf, "bool")) {
        next.type = VAR;
        next.var_type = BOOLEAN;
    } else if (!strcmp(buf, "bin")) {
        next.type = VAR;
        next.var_type = BINARY;
    } else {
        /* not a variable */
        return 0;
    }

    /* was a variable */
    return 1;
}

// jump create_variable
void create_variable(char *name)
{
#ifdef VAR_DEBUG
    printf("Creating variable %s at index %d\n", name, variable_count);
#endif

    /* check if there are too many variables */
    if (variable_count >= 100) {
        fprintf(stderr, "Error %d: could not create variable %s: Variable limit (100) exceeded\n", error_count, name);
        error_count++;
        return;
    }

    /* shorten, since it's used so much */
    register int i = variable_count;

    /* create variable */
    var_list[i].type  = next.var_type;
    var_list[i].name  = malloc(MAXWORD);
    var_list[i].value = NULL;

    /* assign variable name */
    memset(var_list[i].name, 0, MAXWORD);
    memcpy(var_list[i].name, name, strlen(name));

    /* set last-used variable */
    last.var_idx = i;

    /* increment variable count #uselesscomments */
    variable_count++;

#ifdef VAR_DEBUG
    printf("Created variable %s\n", var_list[i].name);
#endif
}

// jump operate
void operate(int idx, char *val, int t)
{
#ifdef OP_DEBUG
    printf("Operate with last_op being %s and idx being %d\n", last_op, idx);
#endif
    struct variable temp;

    /* magic number, string literal, etc. */
    if (t == 1) {
        idx = 44;

        temp = var_list[idx];

        var_list[idx].type  = UNTYPED;
        var_list[idx].value = val;
    }

    char *str = malloc(MAXWORD);

    if (!strcmp(last_op, "--")) {
        var_list[idx].value = ((void *) ((int) var_list[idx].value - 1));
    } else if (!strcmp(last_op, "++")) {
        var_list[idx].value = ((void *) ((int) var_list[idx].value + 1));
    } else if (!strcmp(last_op, "print:")) {
        rkprint(idx);
    } else if (!strcmp(last_op, "read:")) {
        rkread(idx);
    }

    if (t == 1)
        var_list[idx] = temp;

#if RESET_LAST_OP == 1
    memcpy(last_op, "blank", 5);
#endif

    free(str);
}

// jump getval
/* get value from variable or magic number stored in val */
int getval(char *val)
{
    int i;

    i = existing_variable(val);

    if (i != -1) {
        if ((var_list[i].type == INTEGER) || (var_list[i].type == BOOLEAN) || (var_list[i].type == CHARACTER)) {
            return (int) var_list[i].value;
        } else {
            fprintf(stderr, "Invalid type for variable %s: Expected integer, boolean or character\n", var_list[i].name);
            return 0;
        }
    }
    else if ((val[0] == '\'') && (val[2] == '\'')) {
        return val[1];
    }
    else if (isdigit(*val)) {
        return atoi(val);
    }

    fprintf(stderr, "Error: Unrecognized value %s\n", val);
    return 0;
}

// jump existing_variable
int existing_variable(char *name)
{
    int i;

    for (i = 0; i < variable_count; i++)
        if (!strcmp(var_list[i].name, name))
            return i;

    return -1;
}
