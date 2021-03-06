#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "rk-include.h"

#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"

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

void create_variable(char *name)
{
#ifdef VAR_DEBUG
    printf("Creating variable %s at index %d\n", name, variable_count);
#endif

    /* check if there are too many variables */
    if (variable_count >= 100) {
        error(0, "Could not create variable %s: Variable limit (100) exceeded\n", name);
        error_count++;
        return;
    }

    register int i = variable_count;

    /* create variable */
    var_list[i].type  = next.var_type;
    var_list[i].name  = malloc(MAXWORD);
    var_list[i].value = NULL;

    memset(var_list[i].name, 0, MAXWORD);
    memcpy(var_list[i].name, name, strlen(name));

    last.var_idx = i;
    variable_count++;

#ifdef VAR_DEBUG
    printf("Created variable %s\n", var_list[i].name);
#endif
}

void destroy_variable(int idx)
{
    if ((idx < 0) || (idx >= 100))
        return;

    if (existing_variable(var_list[idx].name) == -1) {
        error(0, "Tried to destroy nonexistent variable at index %d\n", idx);
        error_count++;
        return;
    }

    register int i = variable_count;

    /* destroy variable */
    var_list[i].type = 0;
    free(var_list[i].name);

    if ((var_list[i].type == STRING) && (var_list[i].value != NULL))
        free(var_list[i].value);

    variable_count--;
}

void function(signed int idx, char *val)
{
    int m = 0;

    /* magic number, string literal, etc. */
    if (idx == -1) {
        int t_type = next.type;
        int t_vartype = next.var_type;

        set_next_variable("str");
        create_variable("TempVar");

        next.type = t_type;
        next.var_type = t_vartype;

        m = 1;
        idx = last.var_idx;

        var_list[idx].value = malloc(MAXWORD);
        strset(var_list[idx].value, val);
    }

    if (!strcmp(last_func, "print:")) {
        rkprint(idx);
    } else if (!strcmp(last_func, "read:")) {
        rkread(idx);
    }

    if (m)
        destroy_variable(idx);
}

/* get value from variable or magic number stored in val */
int getval(char *val)
{
    int i;

    i = existing_variable(val);

    if (i != -1) {
        if ((var_list[i].type == INTEGER) || (var_list[i].type == BOOLEAN) || (var_list[i].type == CHARACTER)) {
            return (int) var_list[i].value;
        } else {
            error(0, "Invalid type for variable %s: Expected integer, boolean or character\n", var_list[i].name);
            return 0;
        }
    }
    else if ((val[0] == '\'') && (val[2] == '\'')) {
        return val[1];
    }
    else if (isdigit(*val) || ((*val == '-') && isdigit(*(val+1)))) {
        return atoi(val);
    }

    error(0, "Unrecognized value %s\n", val);
    return 0;
}

int existing_variable(char *name)
{
    int i;

    for (i = 0; i < variable_count; i++)
        if (!strcmp(var_list[i].name, name))
            return i;

    return -1;
}

int is_string(char *word)
{
    if ((word[0] == '"') && (word[strlen(word)-1] == '"'))
        return 1;

    int i = existing_variable(word);

    if ((i != -1) && (var_list[i].type == STRING))
        return 1;

    return 0;
}

void unary_operate(int idx, char cr)
{
    switch (cr) {
      case '+': var_list[idx].value = (void *) (((int) var_list[idx].value) + 1); break;
      case '-': var_list[idx].value = (void *) (((int) var_list[idx].value) - 1); break;
      default:  error(0, "Operator %c%c not increment or decrement operator\n", cr, cr);
    }
}
