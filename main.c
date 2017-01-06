#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include "rk-include.h"

FILE *src, *dest;
char *src_fname, *dest_fname;

/*
0 | -o | set output file
1 | 
2 | 
3 | 
4 | 
5 | 
*/

int ARGS[6];
int last_arg;

void parse(char *);
void set_file(char *);
void catch_exit(int);
void main_cleanup(void);

int main(int argc, char *argv[])
{
    int i;  /* used in loops */

    /* init */
    memset(ARGS, 0, sizeof(ARGS));
    src_fname = malloc(MAXWORD);
    dest_fname = malloc(MAXWORD);
    last_arg = -1;

    /* parse arguments */
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            parse(argv[i]);
        } else {
            set_file(argv[i]);
            last_arg = -1;
        }
    }

    /* check for unset files */
    if (!dest) {
        dest = fopen("rk.out", "w");
        strcpy(dest_fname, "rk.out");
    }
    if (!src) {
        if (access(src_fname, R_OK)) {
            int errbackup = errno;
            fprintf(stderr, "Defaulting input to stdin - could not open input file '%s': %s\n", src_fname, strerror(errbackup));
            errno = errbackup;
        }

        src = stdin;
        strcpy(src_fname, "stdin");
    }

    /* test */
#ifdef FILE_DEBUG
    printf("src:  %s\n", src_fname);
    printf("dest: %s\n", dest_fname);
#endif

    /* catch exit signals */
    if (signal(SIGINT, catch_exit) == SIG_ERR)
        fprintf(stderr, "Error: Can't catch SIGINT\n");

    if (signal(SIGTERM, catch_exit) == SIG_ERR)
        fprintf(stderr, "Error: Can't catch SIGTERM\n");

    if (signal(SIGTSTP, catch_exit) == SIG_ERR)
        fprintf(stderr, "Error: Can't catch SIGTSTP\n");

    if (signal(SIGQUIT, catch_exit) == SIG_ERR)
        fprintf(stderr, "Error: Can't catch SIGQUIT\n");

    /* setup variables */
    rk_init();

    /* read file and parse */
    read_loop(src, dest);

    /* clean up */
    rk_cleanup();
    main_cleanup();
    return -error_count;
}

void parse(char *arg)
{
    switch (arg[1]) {
    case 'o': set_arg(0, 1);
    }
}

void set_file(char *filename)
{
    switch (last_arg) {
      case -1:
        src  = fopen(filename, "r");
        strcpy(src_fname, filename);
        break;
      case 0:
        dest  = fopen(filename, "w");
        strcpy(dest_fname, filename);
        break;
    }
}

void main_cleanup(void)
{
    int i;

    /* summary */
    fprintf(dest, "SUMMARY\nVariables: %d\nErrors: %d\n", variable_count, error_count);

    for (i = 0; i < variable_count; i++)
        fprintf(dest, "Variable %d: %s %s = %p\n", i, VARTYPE[var_list[i].type], var_list[i].name, var_list[i].value);
    for (i = 0; i < goto_count; i++)
        fprintf(dest, "Jump %d: %s at %d\n", i, gotos[i].name, (int) gotos[i].offset);

    /* free filenames, flush and close files */
    free(src_fname);
    free(dest_fname);
    fflush(src);
    fflush(dest);
    fclose(src);
    fclose(dest);
}

void catch_exit(int sig)
{
    printf("Caught signal %d, exiting cleanly.\n", sig);
    main_cleanup();
    exit(EXIT_SUCCESS);
}
