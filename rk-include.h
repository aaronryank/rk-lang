#include <stdio.h>

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

/* logic sequence */
struct {
    int in;
    int idx;
    char *keyword;
    char *op[100];
    int (*compute)(void);
    void (*reset)(void), (*add)(char *), (*set)(char *);
} logic;

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

/* mathematical equals operation */
struct eq_keyword {
    int type;
    int val;
};

struct {
    int in;
    int idx;
    int last;
    char *assignment;
    char *op[100];
    int (*compute)(void), (*add)(char *);
    void (*assign)(void), (*reset)(void), (*set)(char *);
} eq;

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

extern int last_val;    /* last value computed */
extern char *last_op;   /* last operator used */

extern int breaklevel; /* the RIDICULOUS while loop problem */

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
extern void rkprint(char *);
extern void rkputchar(char);
extern int  rkread(int);

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
extern void create_variable(char *);
extern void operate(int, char *, int);

/* equation.c functions */
extern int  compute_eq(void);  // eq.compute
extern int  add_eq(char *);    // eq.add
extern void assign_eq(void);   // eq.assign
extern void reset_eq(void);    // eq.reset
extern void set_eq(char *);    // eq.set

/* logic.c functions */
extern int  compute_logic(void);  // logic.compute
extern void reset_logic(void);    // logic.reset
extern void add_logic(char *);    // logic.add
extern void set_logic(char *);    // logic.set

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
#define reset_last_op()      strset(last_op, "blank")
#define remove_quotes(s)     strsnip(s, 0); \
                             strsnip(s, strlen(s)-1)

#define in_range(x, min, max) (x >= min) && (x <= max)
#define logic_end_of(s) (!strcmp(s, "if") ? "fi" : "done")

#define is_reserved_keyword(s)    str_eq(s, RESERVED)
#define is_logic_keyword(s)       str_eq(s, LOGIC_KEYWORDS)
#define is_assignment_operator(s) str_eq(s, ASSIGNMENT_OPERATORS)
#define is_jump_keyword(s)        (strindex(s, "-") == ((signed int) strlen(s)-1))
#define is_function_keyword(s)    (strindex(s, ":") == ((signed int) strlen(s)-1))

#define dummy() // for empty if-else bodies
