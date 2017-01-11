#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma GCC diagnostic ignored "-Wunused-variable"

/* variables */
struct variable {
    int type;
    char *name;
    void *value;
};

/* next unrecognized keyword (variable/function) */
struct {
    int type;
    int var_type;
} next;

/* last used variable */
struct {
    int var_idx;   /* variable index in var_list[] */
} last;

/* logic/equation sequence */
struct {
    int in;
    int idx;
    int last;
    char *keyword;
    char *op[100];
    int (*compute)(void), (*add)(char *);
    void (*assign)(void), (*reset)(void), (*set)(char *), (*logic)(void), (*strmanip)(int), (*remove)(int, int), (*unary_operate)(int);
} compute;

/* unrecognized keyword types */
enum {
    UNDEF_TYPE,
    VAR,
    FUNC,
} unrecognized_types;

/* types of variables */
enum {
    UNTYPED,
    INTEGER,
    CHARACTER,
    STRING,
    BOOLEAN,
    BINARY
} variable_types;

/* types in an equals-operation */
enum {
    UNDEF_EQ,
    OPERATOR,
    VALUE,
    OP_LEFT,
    OP_RIGHT
} eq_keyword_types;

/* jumps */
struct {
    long offset;
    char *name;
} jumps[100];

/* globals */
extern FILE  *src, *dest;     /* source, output */
extern int   ARGS[];          /* command-line argument parsing */
extern int   last_arg;        /* last parsed argument */
extern int   variable_count;  /* total number of variables, also used as an index */
extern int   error_count;     /* total number of errors */
extern int   jump_count;      /* total number of jumps */
extern int   loop_count;      /* total number of loops */
extern long  loop_jump[100];  /* loop jumps */

extern struct variable  var_list[];      /* list of variables */

extern const char  *VARTYPE[];               /* useful constant for determining variable type based on magic (enum) number */
extern const char  *RESERVED[];              /* reserved keywords */
extern const char  *LOGIC_KEYWORDS[];        /* if, then, etc. */
extern const char  *ASSIGNMENT_OPERATORS[];  /* assignment operators */
extern const char  *COMPARISON_OPERATORS[];  /* comparison operators */
extern const char  *SPECIAL_OPERATORS[];     /* . ++ -- etc */

extern int  last_val;   /* last value computed */
extern char *last_func; /* last operator used */

extern int breaklevel;  /* the RIDICULOUS while loop problem */

/* compute.c functions */
extern int  getcmp(char *);
extern void mod_op(int (*)[], int, int);
extern int  compute_compute(void);
extern int  compute_breakout(char *);
extern int  compute_add(char *);
extern void compute_set(char *);
extern void compute_assign(void);
extern void compute_logic(void);
extern void compute_reset(void);
extern void compute_strmanip(int);
extern void compute_remove(int, int);
extern void compute_unary_operate(int);

/* error.c functions */
extern void error(int, char *, ...);

/* main.c functions */
extern void clean_exit(int);

/* strfunc.c functions */
extern void push_char(char, char *);
extern void strsnip(char *, int);
extern void itoa(int, char[]);
extern char *reverse(char *);
extern int getword(FILE *, char *, char *);
extern int strindex(char[], char[]);
extern int chr_eq(int, const char *);
extern int str_eq(const char *, const char **);

/* rk-func.c functions */
extern void rkprint(int);
extern void rkputchar(char);
extern void rkread(int);

/* still don't know where to put this, currently in rk-func.c */
extern void remove_int(int (*)[100], int, int);

/* jump.c functions */
extern void jump(FILE *, char *);
extern void add_jump(FILE *, char *);
extern int  existing_jump(char *);

/* fileops.c functions */
extern void wait_for_character(FILE *, int, int);
extern void break_from(FILE *, char *, char *);

/* variable.c functions */
extern int  set_next_variable(char *);
extern int  existing_variable(char *);
extern int  getval(char *);
extern int  is_string(char *);
extern void create_variable(char *);
extern void function(signed int, char *);
extern void unary_operate(int, char);

/* rk-main.c functions */
extern void rk_init(void);
extern void rk_cleanup(void);
extern void rk_parse(FILE *, FILE *, char *);

/* readfile.c functions */
extern void read_loop(FILE *, FILE *);

/* macros */
#define MAXWORD 1024
#define RESET_LAST_OP 0

#define odd(x)  (x % 2)
#define even(x) !odd(x)
#define set_arg(idx, val)    do { ARGS[idx] = val; last_arg = idx; } while (0)
#define set_eq_op_idx(x, v)  eq.op[eq.idx] = malloc(MAXWORD); \
                             eq.last       = x;               \
                             strset(eq.op[eq.idx], v)
#define strset(s, buf)       memset(s, 0, MAXWORD);       \
                             memcpy(s, buf, strlen(buf))
#define reset_last_func()    strset(last_func, "blank")
#define remove_quotes(s)     strsnip(s, 0); \
                             strsnip(s, strlen(s)-1)

#define in_range(x, min, max) (x >= min) && (x <= max)
#define logic_end_of(s) (!strcmp(s, "if") ? "fi" : "done")

#define is_reserved_keyword(s)    str_eq(s, RESERVED)
#define is_logic_keyword(s)       str_eq(s, LOGIC_KEYWORDS)
#define is_assignment_operator(s) str_eq(s, ASSIGNMENT_OPERATORS)
#define is_special_operator(s)    str_eq(s, SPECIAL_OPERATORS)
#define is_jump_keyword(s)        ((strindex(s, "-") == ((signed int) strlen(s)-1)) && (s[0] != '-'))
#define is_function_keyword(s)    (strindex(s, ":") == ((signed int) strlen(s)-1))
#define is_unary_operator(s)      (!strcmp(s, "++") || !strcmp(s, "--"))
#define is_equation_operator(s)   (chr_eq(*s, "+-x/&%^") && (*(s+1) == '\0'))
#define is_logical_operator(s)    (str_eq(s, COMPARISON_OPERATORS))
#define is_operator(s)            (is_special_operator(s) || is_equation_operator(s) || is_logical_operator(s))
#define is_logical_complement(s)  ((!strcmp(s, "then") && !strcmp(compute.keyword, "then")) || (!strcmp(s, "do") && !strcmp(compute.keyword, "while")))

#define getwhilepos()  (ftell(src) - 6)
#define add_loop()     loop_jump[loop_count++] = getwhilepos()

#define dummy() // for empty if-else bodies
