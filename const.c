#include "rk-include.h"

const char *VARTYPE[] = {
    "UNTYPED  ",
    "INTEGER  ",
    "CHARACTER",
    "STRING   ",
    "BOOLEAN  ",
    "BINARY   "
};

const char *RESERVED[] = {
    "var", "int", "key", "str", "bool", "bin",
    "for", "in", "if", "then", "fi", "while",
    "poo-nuggets", /* o_O */
    ":done"
};

const char *LOGIC_KEYWORDS[] = {"for", "if", "while"};
const char *COMPARISON_OPERATORS[] = {"==", ">=", "<=", "!=", ">", "<", "||", "&&"};
const char *ASSIGNMENT_OPERATORS[] = {"=", "+=", "-=", "x=", "/=", "%=", "&=", "^=", "|=", "<<=", ">>="};
