/*! \file
 * This file implements the Read-Eval-Print Loop (REPL) for the simple CS24
 * Python interpreter.
 */

#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "eval.h"
#include "global.h"
#include "alloc.h"
#include "parse.h"


#define SUBPYTHON_HISTORY ".subpython"

#define DEFAULT_MEMORY_SIZE 1024

static int verbose = 0;


/*!
 * This is the Read-Eval-Print-Loop (aka "REPL") function.
 */
void read_eval_print_loop() {
    char *line;

    // Load command history, if any
    read_history(SUBPYTHON_HISTORY);

    // Disable filename auto-complete on TAB key
    rl_bind_key ('\t', rl_insert);

    while (true) {
        line = readline("> ");
        if (line == NULL) {
            break;   // End of file
        } else if (line[0] != 0) {
            // Not a blank line, so record it in history.
            add_history(line);

            // This is wasteful; we'd prefer to just write the most recent
            // command to history, but append_history() isn't always available.
            write_history(SUBPYTHON_HISTORY);
        }

        if (setjmp(error_jmp)) {
            goto free_loop;
        }

        ParseStatement *stmt = read_statement_from_string(line);

        if (stmt == NULL) {
            goto free_loop;
        }

        eval_stmt(stmt);

        if (verbose) {
            printf("\n");
            
            print_globals();

            printf("\nMemory Contents:\n");
            memdump();

            printf("\n");
        }

free_loop:
        free(line);
        parse_free_all();
    }
    printf("\nQuitting, goodbye.\n");
}


/*! Prints the program's usage information. */
void usage(char *program) {
    printf("usage:  %s [-m memory_size] [-v]\n\n", program);
    printf("\tRuns the CS24 Sub-Python interpreter\n\n");
    printf("\t-m memory_size specifies the size of the memory pool to use\n\n");
    printf("\t-v causes the REPL loop to run in verbose mode, printing\n");
    printf("\tout the current bindings and memory contents after every\n");
    printf("\tevaluation.\n");
}


/*!
 * The entry point to the SubPython interpreter.  Several command-line
 * arguments can be specified, to control various aspects of the program
 * behavior.
 */
int main(int argc, char **argv) {
    int memory_size = DEFAULT_MEMORY_SIZE;
    int c;

    while ((c = getopt(argc, argv, "m:v")) != -1) {
        switch (c) {
            case 'm':
                memory_size = atoi(optarg);
                if (memory_size <= 0) {
                    printf("Error:  memory size must be > 0\n");
                    usage(argv[0]);
                    exit(1);
                }
                break;

            case 'v':
                verbose = 1;
                break;

            case '?':
                usage(argv[0]);
                exit(1);

            default:
                abort();
        }
    }

    printf("Using a memory size of %d bytes.\n", memory_size);

    init_alloc(memory_size);
    read_eval_print_loop();
    close_alloc();

    return 0;
}

