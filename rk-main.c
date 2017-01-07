#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rk-include.h"

int error_count = 0, variable_count = 0, jump_count = 0;
int last_val = 0;
int loop_count = 0;
struct variable var_list[100];
long loop_jump[100];
char *last_func;

void rk_init(void)
{
    /* allocate space for last_func */
    last_func = malloc(MAXWORD);
    memset(last_func, 0, MAXWORD);
    memcpy(last_func, "blank", 5);

    /* initialize variable list */
    memset(var_list, 0, sizeof(var_list));

    /* initialize jump list */
    memset(jumps, 0, sizeof(jumps));

    /* initialize while list */
    memset(loop_jump, 0L, sizeof(loop_jump));

    /* initialize eq op */
    eq.compute    = compute_eq;
    eq.assign     = assign_eq;
    eq.reset      = reset_eq;
    eq.add        = add_eq;
    eq.set        = set_eq;
    eq.idx        = eq.in = eq.last = 0;
    eq.assignment = malloc(MAXWORD);

    /* initialize logic */
    memset(logic.op, 0, sizeof(logic.op));
    logic.compute = compute_logic;
    logic.reset   = reset_logic;
    logic.add     = add_logic;
    logic.set     = set_logic;
    logic.in      = logic.idx = 0;
    logic.keyword = malloc(MAXWORD);
}

void rk_cleanup(void)
{
    int i;

    free(last_func);
    free(eq.assignment);
    free(logic.keyword);

    for (i = 0; i < variable_count; i++) {
        free(var_list[i].name);
        if (var_list[i].type == STRING)
            free(var_list[i].value);
    }

    for (i = 0; i < jump_count; i++)
        free(jumps[i].name);
}

void rk_parse(FILE *src, FILE *dest, char *buf)
{
    int i;

    /* check for empty string */
    if (!strcmp(buf, ""))
        return;

#ifdef PARSE_DEBUG
    printf("---CHECKING \e[31m%s\e[0m---\n", buf);
#endif

    /* check for comments */
    switch (*buf) {
      case '*':
        wait_for_character(src, '\n', 0);
        return;
      case ';':
        wait_for_character(src, ';', 0);
        return;
    }

#ifdef PARSE_DEBUG
    printf("---PARSING  \e[32m%s\e[0m---\n", buf);
#endif

    /* end of code */
    if (!strcmp(buf, "rk:end")) {
        wait_for_character(src, -1, 0);
    }

    /* variable type declaration */
    else if (set_next_variable(buf)) {
        reset_last_func();
    }

    /* assignment operator, triggers eq equation */
    else if (is_assignment_operator(buf) && (eq.in == 0)) {
        eq.set(buf);
        reset_last_func();
    }

    /* logic keyword, triggers logic evaluation */
    else if (is_logic_keyword(buf) && (logic.in == 0)) {
        logic.set(buf);
        reset_last_func();
    }

    /* add buf to logic */
    else if (logic.in == 1) {
        logic.add(buf);
    }

    /* add buf to equation */
    else if ((eq.in == 1) && !eq.add(buf)) {
        dummy();
    }

    /* string literals */
    else if ((buf[0] == '\"') && (buf[strlen(buf)-1] == '\"')) {
        remove_quotes(buf);
        function(-1, buf);
    }

    /* functions */
    else if (is_function_keyword(buf)) {
        strset(last_func, buf);
    }

    /* repeat the loop */
    else if (!strcmp(buf, "done")) {
        fseek(src, loop_jump[loop_count-1], SEEK_SET);
    }

    /* go to the jump */
    else if (existing_jump(buf) != -1) {
        reset_last_func();
        jump(src, buf);
    }

    /* jump (goto) declaration */
    else if (is_jump_keyword(buf)) {
        strset(last_func, "blank");
        add_jump(src, buf);
    }

    /* existing variable */
    else if ((i = existing_variable(buf)) != -1) {
        last.var_idx = i;
        if (strcmp(last_func, "blank"))
            function(i, 0);
    }

    /* nonexistent variable */
    else {
        create_variable(buf);
        reset_last_func();
    }
}
