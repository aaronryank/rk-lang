#include <stdio.h>
#include <string.h>
#include "rk-include.h"

/* check if a is equal to anything in b */
int chr_eq(int a, const char *b)
{
    int i, s = strlen(b);
    for (i = 0; i < s; i++)
        if (a == b[i])
            return 1;
    return 0;
}

/* check if a is equal to anything in b */
int str_eq(const char *a, const char **b)
{
    while (*b)
        if (!strcmp(a, *b++))  /* ooh, taking advantage of the increment operator rules... #savage */
            return 1;
    return 0;
}

/* get 'word' from f (ending in any of the characters in delim) and store it in buf */
int getword(FILE *f, char *buf, char *delim)
{
    if (!f)
        return 0;

    int c, i;
    i = c = 0;

    if ((c = getc(f)) == EOF)
        return 0;
    else
        ungetc(c, f);

    while (((c = getc(f)) != EOF) && (!chr_eq(c, delim)) && (i < MAXWORD))
        buf[i++] = c;
    buf[i++] = '\0';

    return i;
}

// strindex from K&R: search for a string in a string
int strindex(char s[], char t[])
{
    int i, j, k, last;

    last = -1;
    for (i = 0; s[i] != '\0'; i++) {
        for (j = i, k = 0; t[k] != '\0' && s[j] == t[k]; j++, k++)
            continue;

        if (k > 0 && t[k] == '\0')
            last = i;
    }

    return last;
}

/* itoa from "The C Programming Language Second Edition" (aka K&R) */
void itoa(int n, char s[])
{
    int i, sign;
    if ((sign = n) < 0)        /* record sign */
        n = -n;                /* make n positive */
    i = 0;
    do {                       /* generate digits in reverse order */
        s[i++] = n % 10 + '0'; /* get next digit */
    } while ((n /= 10) > 0);   /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
    return;
}

/* apparently I can't write a freaking string-reverse algorithm
   stackoverflow.com/a/784567/6850771 */
char *reverse(char *str)
{
    char tmp, *src, *dst;
    size_t len;

    /* this is inefficient as he'll */
    if (str != NULL)
    {
        len = strlen(str);
        if (len > 1) {
            src = str;
            dst = src + len - 1;
            while (src < dst) {
                tmp = *src;
                *src++ = *dst;
                *dst-- = tmp;
            }
        }
    }
    return str;
}

/* remove str[idx] */
void strsnip(char *str, int idx)
{
    int i, l = strlen(str);

    for (i = idx; i < l; i++)
        str[i] = str[i+1];
}

/* push char c to beginning of string s */
void push_char(char c, char *s)
{
    int i, l = strlen(s);

    for (i = l; i; i--)
        s[i] = s[i-1];

    s[0] = c;
}
