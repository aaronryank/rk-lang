#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "rk-include.h"

#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"

enum { NOTHING, MULT, DIV, MOD, PLUS, MINUS, AND, OR, XOR };
//               x     /    %    +      -     &   |    ^

// jump add_op
struct eq_keyword add_op(int type, int chrval)
{
    struct eq_keyword temp;

    temp.type = type;
    temp.val  = chrval;

    return temp;
}

// jump assign_eq
void assign_eq(void)
{
    /* shorten, since they're used so much */
    const char *a  = eq.assignment;
    const int i    = last.var_idx;
    const int t    = var_list[i].type;

    if ((t == INTEGER) || (t == CHARACTER) || (t == BOOLEAN))
    {
        const int ival = eq.compute();

        if (!strcmp(a, "="))
            var_list[i].value = (void *) ival;
        else if (!strcmp(a, "+="))
            var_list[i].value = ((void *) (((int) var_list[i].value) + ival));
        else if (!strcmp(a, "-="))
            var_list[i].value = ((void *) (((int) var_list[i].value) - ival));
        else if (!strcmp(a, "x="))
            var_list[i].value = ((void *) (((int) var_list[i].value) * ival));
        else if (!strcmp(a, "/="))
            var_list[i].value = ((void *) (((int) var_list[i].value) / ival));
        else if (!strcmp(a, "%="))
            var_list[i].value = ((void *) (((int) var_list[i].value) % ival));
        else if (!strcmp(a, "&="))
            var_list[i].value = ((void *) (((int) var_list[i].value) & ival));
        else if (!strcmp(a, "^="))
            var_list[i].value = ((void *) (((int) var_list[i].value) ^ ival));
        else if (!strcmp(a, "|="))
            var_list[i].value = ((void *) (((int) var_list[i].value) | ival));
        else if (!strcmp(a, "<<="))
            var_list[i].value = ((void *) (((int) var_list[i].value) << ival));
        else if (!strcmp(a, ">>="))
            var_list[i].value = ((void *) (((int) var_list[i].value) >> ival));

        if (t == BOOLEAN)
            var_list[i].value = (void *) !!((int) var_list[i].value);

    }
    else if (t == STRING)
    {
        if (eq.idx > 1) {
            fprintf(stderr, "Error: Too many arguments to string '%s' assignment\n", var_list[i].name);
            return;
        }

        int idx;
        char *val = malloc(MAXWORD);
        strset(val, eq.op[0]);

        if ((val[0] == '"') && (val[strlen(val)-1] == '"')) {
            strsnip(val, 0);
            strsnip(val, strlen(val)-1);
        }
        else if ((idx = existing_variable(val))) {
            if (var_list[idx].type == STRING) {
                strset(val, var_list[idx].value);
            } else {
                fprintf(stderr, "Error assigning string %s to non-string %p\n", val, var_list[idx].value);
            }
        } else {
            fprintf(stderr, "Error: variable/string %s not found\n", val);
        }

        var_list[i].value = malloc(MAXWORD);
        memset(var_list[i].value, 0, MAXWORD);
        memcpy(var_list[i].value, val, strlen(val));

        free(val);
    }
}

// jump reset_eq
/* reset eq variables */
void reset_eq(void)
{
    int i;
    for (i = 0; i < eq.idx; i++)
        free(eq.op[i]);

    eq.in = eq.last = eq.idx = 0;
}

// jump geteq
int geteq(char *val)
{
    if (chr_eq(*val, "x/%+-&|^") && (*(val+1) == 0)) {
        switch (*val) {
          case 'x': return MULT;
          case '/': return DIV;
          case '%': return MOD;
          case '+': return PLUS;
          case '-': return MINUS;
          case '&': return AND;
          case '|': return OR;
          case '^': return XOR;
          default:  return NOTHING;
        }
    } else {
        return getval(val);
    }
}

void mod_eq_op(int (*op)[], int idx, int order)
{
    if (order == 1) {
        if ((*op)[idx] == MULT)
            (*op)[idx-1] *= (*op)[idx+1];
        else if ((*op)[idx] == DIV)
            (*op)[idx-1] /= (*op)[idx+1];
        else if ((*op)[idx] == MOD)
            (*op)[idx-1] %= (*op)[idx+1];
    }

    else if (order == 2) {
        if ((*op)[idx] == PLUS)
            (*op)[idx-1] += (*op)[idx+1];
        else if ((*op)[idx] == MINUS)
            (*op)[idx-1] -= (*op)[idx+1];
    }

    else if (order == 3) {
        if ((*op)[idx] == AND)
            (*op)[idx-1] &= (*op)[idx+1];
        else if ((*op)[idx] == OR)
            (*op)[idx-1] |= (*op)[idx+1];
        else if ((*op)[idx] == XOR)
            (*op)[idx-1] ^= (*op)[idx+1];
    }
}

// jump compute_eq
int compute_eq(void)
{
    int i, j, s, val;
    int op[100] = {0};

    s = eq.idx;

    for (i = 0; i < s; i++)
        op[i] = geteq(eq.op[i]);

#ifdef EQ_DEBUG
    printf("\nReached eq with "); 
    for (j = 0; j < s; j++)
        printf("%d ", op[j]);
    putchar('\n');
#endif

    for (i = 1; i <= 3; i++) {
        for (j = 1; j <= s; j += 2) {
            if ((in_range(op[j], MULT, MOD) && (i == 1))   ||
                (in_range(op[j], PLUS, MINUS) && (i == 2)) ||
                (in_range(op[j], AND, XOR) && (i == 3)))   {

                mod_eq_op(&op, j, i);
                remove_int(&op, j, 2);
                s -= 2;
                j -= 2;
            }
        }
    }

#ifdef EQ_DEBUG
    printf("Finished with ");
    for (j = 0; j < s; j++)
        printf("%d ", op[j]);
    putchar('\n');
    printf("Returning %d\n", op[0]);
#endif

    return op[0];
}

// jump add_eq
int add_eq(char *buf)
{
#ifdef EQ_DEBUG
    printf("In eq with %s\n", buf);
#endif
    int eq_break = 0;

    if (chr_eq(*buf, "+-x/%&|^") && (*(buf+1) == 0)) {
#ifdef EQ_DEBUG
        printf("Operator- %s\n", buf);
#endif
        if (eq.last == OPERATOR) {
            eq_break = 1;
        } else {
            set_eq_op_idx(OPERATOR, buf);
        }
    }
    else if (is_reserved_keyword(buf) || is_function_keyword(buf)) {
#ifdef EQ_DEBUG
        printf("Reserved/function keyword- %s\n", buf);
#endif
        eq_break = 1;
    }
    else { /* magic number, variable, etc. */
#ifdef EQ_DEBUG
        printf("Other- %s\n", buf);
#endif
        if (eq.last == VALUE) {
            eq_break = 1;
        } else {
            set_eq_op_idx(VALUE, buf);
        }
    }

    if (eq_break) {
        eq.assign();
        eq.reset();
        return 1;
    } else {
        eq.idx++;
        return 0;
    }
}

void set_eq(char *buf)
{
    eq.in = 1;
    strset(eq.assignment, buf);
}
