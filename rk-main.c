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

    /* initialize compute */
    memset(compute.op, 0, sizeof(compute.op));
    compute.compute  = compute_compute;
    compute.assign   = compute_assign;
    compute.reset    = compute_reset;
    compute.add      = compute_add;
    compute.set      = compute_set;
    compute.logic    = compute_logic;
    compute.strmanip = compute_strmanip;
    compute.remove   = compute_remove;
    compute.idx      = compute.in = 0;
    compute.keyword  = malloc(MAXWORD);
}

void rk_cleanup(void)
{
    int i;

    free(last_func);
    free(compute.keyword);

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

#ifdef DEBUG_STEP
    usleep(100000);
#endif

    /* end of code */
    if (!strcmp(buf, "rk:end")) {
        wait_for_character(src, -1, 0);
    }

    /* variable type declaration */
    else if (set_next_variable(buf)) {
        reset_last_func();
    }

    else if ((compute.in == 1) && compute.add(buf)) {
        dummy();
    }

    else if ((is_assignment_operator(buf) || is_logic_keyword(buf)) && (compute.in == 0)) {
        compute.set(buf);
        reset_last_func();
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
        reset_last_func();
        fseek(src, loop_jump[loop_count-1], SEEK_SET);
    }

    /* go to the jump */
    else if (existing_jump(buf) != -1) {
        reset_last_func();
        jump(src, buf);
    }

    /* jump (goto) declaration */
    else if (is_jump_keyword(buf)) {
        reset_last_func();
        add_jump(src, buf);
    }

    /* stop creation of 'fi' and 'done' variables */
    else if (is_reserved_keyword(buf)) {
        dummy();
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
