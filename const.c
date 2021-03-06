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
    "for", "in", "if", "then", "fi", "while", "do", "done",
    "poo-nuggets", /* o_O */
};

const char *LOGIC_KEYWORDS[] = {"for", "if", "while"};
const char *COMPARISON_OPERATORS[] = {"==", ">=", "<=", "!=", ">", "<", "||", "&&"};
const char *ASSIGNMENT_OPERATORS[] = {"=", "+=", "-=", "x=", "/=", "%=", "&=", "^=", "|=", "<<=", ">>="};
const char *SPECIAL_OPERATORS[] = {"."}; // that's all currently
