#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "rk-include.h"

#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"

// jump reset_logic
/* reset logic variables */
void reset_logic(void)
{
    int i;

    for (i = 0; i < logic.idx; i++)
        free(logic.op[i]);

    logic.idx = logic.in = 0;

    memset(logic.keyword, 0, MAXWORD);
}

// jump getop
/* get operator index from val */
int getop(char *val)
{
    enum {
        UNDEF, LT, GT, LEQ, GEQ, EQ, NEQ, AND, OR
    };

    if (!strcmp(val, "<"))
        return LT;
    else if (!strcmp(val, ">"))
        return GT;
    else if (!strcmp(val, "<="))
        return LEQ;
    else if (!strcmp(val, ">="))
        return GEQ;
    else if (!strcmp(val, "=="))
        return EQ;
    else if (!strcmp(val, "!="))
        return NEQ;
    else if (!strcmp(val, "&&"))
        return AND;
    else if (!strcmp(val, "||"))
        return OR;
    return UNDEF;
}

// jump compute_logic
int compute_logic(void)
{
    enum {
        UNDEF, LT, GT, LEQ, GEQ, EQ, NEQ, AND, OR
    /*         <   >   <=   >=   ==  !=   &&   || */
    };

    int val[100]  = {0};
    int i;

#ifdef LOGIC_DEBUG
    for (i = 0; i < logic.idx; i++)
        printf("%s ", logic.op[i]);
    putchar('\n');
#endif

    for (i = 0; i < logic.idx; i++)
        val[i] = (odd(i) ? getop(logic.op[i]) : getval(logic.op[i]));

#ifdef LOGIC_DEBUG
    for (i = 0; i < logic.idx; i++)
        printf("%d ", val[i]);
    putchar('\n');
#endif

    for (i = 1; i < logic.idx; i += 2) {

        if (val[i] == LT)
            val[i-1] = (val[i-1] < val[i+1]);
        else if (val[i] == GT)
            val[i-1] = (val[i-1] > val[i+1]);
        else if (val[i] == LEQ)
            val[i-1] = (val[i-1] <= val[i+1]);
        else if (val[i] == GEQ)
            val[i-1] = (val[i-1] >= val[i+1]);

        if ((val[i] > UNDEF) && (val[i] < EQ)) {
            remove_int(&val, i, 2);
            i = 1;
            logic.idx -= 2;
        }
    }

    for (i = 1; i < logic.idx; i += 2) {

        if (val[i] == EQ)
            val[i-1] = (val[i-1] == val[i+1]);
        else if (val[i] == NEQ)
            val[i-1] = (val[i-1] != val[i+1]);

        if ((val[i] > GEQ) && (val[i] < AND)) {
            remove_int(&val, i, 2);
            i = 1;
            logic.idx -= 2;
        }
    }

    for (i = 1; i < logic.idx; i += 2) {

        if (val[i] == AND)
            val[i-1] = (val[i-1] && val[i+1]);
        else if (val[i] == OR)
            val[i-1] = (val[i-1] || val[i+1]);

        if ((val[i] == AND) || (val[i] == OR)) {
            remove_int(&val, i, 2);
            i = 1;
            logic.idx -= 2;
        }
    }

#ifdef LOGIC_DEBUG
    for (i = 0; i < logic.idx; i++)
        printf("%d ", val[i]);
    printf("\nReturning %d\n", val[0]);
#endif

    return val[0];
}

// jump add_logic
void add_logic(char *buf)
{
    if ((!strcmp(buf, "then") && !strcmp(logic.keyword, "if")) ||
        (!strcmp(buf, "do") && !strcmp(logic.keyword, "while"))) {

        /* check result */
        if (logic.compute() == 0) {
            breaklevel = 0;
            break_from(src, logic.keyword, logic_end_of(logic.keyword));

            if (!strcmp(logic.keyword, "while"))
                loop_count--;
        }

        logic.reset();
    } else {
        logic.op[logic.idx] = malloc(MAXWORD);
        strset(logic.op[logic.idx], buf);
        logic.idx++;
    }
}

// jump set_logic
void set_logic(char *buf)
{
    logic.in = 1;
    strset(logic.keyword, buf);

    if (!strcmp(logic.keyword, "while")) {
        if (loop_jump[loop_count-1] != (ftell(src) - 6)) {
             loop_jump[loop_count]  = ftell(src);
             loop_jump[loop_count] -= 6; // so that it reads the 'while' again
             loop_count++;
        }
    }
}
