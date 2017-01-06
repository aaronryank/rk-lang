#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "rk-include.h"

#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"

enum { UNDEF, LT, GT, LEQ, GEQ, EQ, NEQ, AND, OR };
/*            <   >   <=   >=   ==  !=   &&   || */


// jump reset_logic     reset logic variables
void reset_logic(void)
{
    int i;

    for (i = 0; i < logic.idx; i++)
        free(logic.op[i]);

    logic.idx = logic.in = 0;

    memset(logic.keyword, 0, MAXWORD);
}

// jump getop      get operator index from val
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
    else
        return getval(val);
}

void mod_logic_op(int (*op)[], int idx, int order)
{
    if (order == 1) {
        if ((*op)[idx] == LT)
            (*op)[idx-1] = ((*op)[idx-1] < (*op)[idx+1]);
        else if ((*op)[idx] == GT)
            (*op)[idx-1] = ((*op)[idx-1] > (*op)[idx+1]);
        else if ((*op)[idx] == LEQ)
            (*op)[idx-1] = ((*op)[idx-1] <= (*op)[idx+1]);
        else if ((*op)[idx] == GEQ)
            (*op)[idx-1] = ((*op)[idx-1] >= (*op)[idx+1]);
    }

    else if (order == 2) {
        if ((*op)[idx] == EQ)
            (*op)[idx-1] = ((*op)[idx-1] == (*op)[idx+1]);
        else if ((*op)[idx] == NEQ)
            (*op)[idx-1] = ((*op)[idx-1] != (*op)[idx+1]);
    }

    else if (order == 3) {
        if ((*op)[idx] == AND)
            (*op)[idx-1] = ((*op)[idx-1] && (*op)[idx+1]);
        else if ((*op)[idx] == OR)
            (*op)[idx-1] = ((*op)[idx-1] || (*op)[idx+1]);
    }
}

// jump compute_logic
int compute_logic(void)
{
    int op[100] = {0};
    int i, j, s;

    s = logic.idx;

#ifdef LOGIC_DEBUG
    for (i = 0; i < s; i++)
        printf("%s ", logic.op[i]);
    putchar('\n');
#endif

    for (i = 0; i < s; i++)
        op[i] = getop(logic.op[i]);

#ifdef LOGIC_DEBUG
    for (i = 0; i < s; i++)
        printf("%d ", op[i]);
    putchar('\n');
#endif

    for (i = 1; i <= 3; i++) {
        for (j = 1; j <= s; j += 2) {
            if ((in_range(op[j], LT, GEQ) && (i == 1))   ||
                (in_range(op[j], EQ, NEQ) && (i == 2))   ||
                (in_range(op[j], AND, OR) && (i == 3)))  {

                mod_logic_op(&op, j, i);
                remove_int(&op, j, 2);
                s -= 2;
                j -= 2;
            }
        }
    }

#ifdef LOGIC_DEBUG
    for (i = 0; i < logic.idx; i++)
        printf("%d ", val[i]);
    printf("\nReturning %d\n", val[0]);
#endif

    return op[0];
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
