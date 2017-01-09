#include "rk-include.h"
#include <stdio.h>

#define is_equation_operator(s) (chr_eq(*s, "+-x/&%^") && (*(s+1) == '\0'))
#define is_logical_operator(s)  (str_eq(s, COMPARISON_OPERATORS))

#define is_logical_complement(s) ((!strcmp(s, "then") && !strcmp(compute.keyword, "then")) || \
                                 (!strcmp(s, "do") && !strcmp(compute.keyword, "while")))

enum { UNDEF, LT, GT, LEQ, GEQ, EQ, NEQ, L_AND, L_OR, MULT, DIV, MOD, PLUS, MINUS, AND, OR, XOR } eq_types;

void compute_set(char *keyword)
{
    compute.in = 1;
    strset(compute.keyword, keyword);

    if (!strcmp(keyword, "while")) {
        if (loop_jump[loop_count-1] != (ftell(src) - 6)) {
            loop_jump[loop_count] = ftell(src) - 6;
            loop_count++;
        }
    }
}

int compute_breakout(char *keyword)
{
    if (is_equation_operator(keyword) || is_logical_operator(keyword)) {
        if (compute.last == OPERATOR) {
            return 1;
        } else {
            compute.last = OPERATOR;
        }
    }
    else if (is_reserved_keyword(keyword) || is_function_keyword(keyword) || is_logical_complement(keyword)) {
        return 1;
    }
    else {
        if (compute.last == VALUE) {
            return 1;
        } else {
            compute.last = VALUE;
        }
    }

    return 0;
}

// oops. #badnamingconventions
int compute_compute(void)
{
    int i, j, s, val, type;
    int op[100] = {0};

    s = compute.idx;

    for (i = 0; i < s; i++)
        op[i] = getcmp(compute.op[i]);

#ifdef COMPUTE_DEBUG
    printf("Computing ");
    for (i = 0; i < s; i++)
        printf("%d ", op[i]);
    putchar('\n');
#endif

    for (type = 1; type <= 2; type++) {
        for (i = 1; i <= 3; i++) {
            for (j = 1; j <= s; j += 2) {
                if (((type == 1) &&
                    ((in_range(op[j], MULT, MOD) && (i == 1))   ||
                     (in_range(op[j], PLUS, MINUS) && (i == 2)) ||
                     (in_range(op[j], AND, XOR) && (i == 3)))) ||
                    ((type == 2) &&
                    ((in_range(op[j], LT, GEQ) && (i == 1)) ||
                     (in_range(op[j], EQ, NEQ) && (i == 2)) ||
                    (in_range(op[j], AND, OR) && (i == 3))))) {

                    mod_op(&op, j, i);
                    remove_int(&op, j, 2);
                    s -= 2;
                    j -= 2;
                }
            }
        }
    }

    return op[0];
}

void compute_logic(void)
{
    const int ival = compute.compute();

    if (ival == 0) {
        breaklevel = 0;
        break_from(src, compute.keyword, logic_end_of(compute.keyword));

        if (!strcmp(compute.keyword, "while"))
            loop_count--;
    }
}

int compute_add(char *keyword)
{
#ifdef COMPUTE_DEBUG
    printf("Compute adding %s\n", keyword);
#endif

    if (compute_breakout(keyword)) {
        if (is_assignment_operator(compute.keyword))
            compute.assign();
        else
            compute.logic();

        compute.reset();

        return 0;
    } else {
        compute.op[compute.idx] = malloc(MAXWORD);
        strset(compute.op[compute.idx], keyword);
        compute.idx++;
        return 1;
    }
}

void compute_assign(void)
{
#ifdef COMPUTE_DEBUG
    printf("Assigning\n");

    int ii;
    for (ii = 0; ii < compute.idx; ii++)
        printf("%s ", compute.op[ii]);
    putchar('\n');
#endif

    /* shorten, since they're used so much */
    const char *a  = compute.keyword;
    const int i    = last.var_idx;
    const int t    = var_list[i].type;

    if ((t == INTEGER) || (t == CHARACTER) || (t == BOOLEAN))
    {
        const int ival = compute.compute();

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
        if (compute.idx > 1) {
            fprintf(stderr, "Error: Too many arguments to string '%s' assignment\n", var_list[i].name);
            return;
        }

        int idx;
        char *val = malloc(MAXWORD);
        strset(val, compute.op[0]);

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

void compute_reset(void)
{
    int i;

    for (i = 0; i < compute.idx; i++)
        free(compute.op[i]);

    compute.idx = compute.in = compute.last = 0;

    memset(compute.keyword, 0, MAXWORD);
}

int getcmp(char *val)
{
    if (chr_eq(*val, "x/%+-&|^<>") && (*(val+1) == 0)) {
        switch (*val) {
          case 'x': return MULT;
          case '/': return DIV;
          case '%': return MOD;
          case '+': return PLUS;
          case '-': return MINUS;
          case '&': return AND;
          case '|': return OR;
          case '^': return XOR;
          case '>': return GT;
          case '<': return LT;
          default:  return UNDEF;
        }
    } else if (!strcmp(val, "<=")) {
        return LEQ;
    } else if (!strcmp(val, ">=")) {
        return GEQ;
    } else if (!strcmp(val, "==")) {
        return EQ;
    } else if (!strcmp(val, "!=")) {
        return NEQ;
    } else if (!strcmp(val, "&&")) {
        return L_AND;
    } else if (!strcmp(val, "||")) {
        return L_OR;
    } else {
        return getval(val);
    }
}

void mod_op(int (*op)[], int idx, int order)
{
    int pre        = (*op)[idx-1];
    const int cur  = (*op)[idx];
    const int post = (*op)[idx+1];

    if (order == 1) {
        /* equation */
        if (cur == MULT)
            pre *= post;
        else if (cur == DIV)
            pre /= post;
        else if (cur == MOD)
            pre %= post;

        /* logic */
        else if (cur == LT)
            pre = (pre < post);
        else if (cur == GT)
            pre = (pre > post);
        else if (cur == LEQ)
            pre = (pre <= post);
        else if (cur == GEQ)
            pre = (pre >= post);
    }

    else if (order == 2) {
        /* equation */
        if (cur == PLUS)
            pre += post;
        else if (cur == MINUS)
            pre -= post;

        /* logic */
        else if (cur == EQ)
            pre = (pre == post);
        else if (cur == NEQ)
            pre = (pre != post);
    }

    else if (order == 3) {
        /* equation */
        if (cur == AND)
            pre &= post;
        else if (cur == OR)
            pre |= post;
        else if (cur == XOR)
            pre ^= post;

        /* logic */
        else if (cur == L_AND)
            pre = (pre && post);
        else if (cur == L_OR)
            pre = (pre || post);
    }

    (*op)[idx-1] = pre;
}
