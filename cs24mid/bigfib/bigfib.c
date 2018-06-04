#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "bigint.h"


static int verbose = 0;


/* Simple helper to print out the nth Fibonacci number. */
void print_fib(int n, bigint_t *fibn) {
    printf("fib(%d) = ", n);
    print_bigint(fibn);
    printf("\n");
}


/* Computes the n-th value in the Fibonacci sequence, where fib(0) = 0,
 * fib(1) = 1, and fib(n) = fib(n-2) + fib(n-1) for n > 1.
 *
 * This computation is performed using unsigned bigints, and the result is
 * returned as an unsigned bigint, since the result can very easily overflow
 * a 32-bit or 64-bit unsigned integer.
 *
 * If the function encounters an allocation failure or an overflow, it prints
 * out an error message and then abort()s the entire program.
 */
bigint_t * big_fib(int n, uint8_t size) {
    bigint_t *fibn_2, *fibn_1, *fibn;
    int i;

    assert(size > 0);

    // Handle fib(0) and fib(1) separately, to simplify the
    // rest of the logic.  It's a bit gross how we do this,
    // but I'm trying to avoid allocating space until we
    // know we need it, to avoid having to free things.

    fibn_1 = init_bigint(size);

    if (verbose)
        print_fib(0, fibn_1);

    if (n == 0)
        return fibn_1; // fibn_1 = 0

    set_bigint_value(1, fibn_1);

    if (verbose)
        print_fib(1, fibn_1);

    if (n == 1)
        return fibn_1; // fibn_1 = 1

    fibn_2 = init_bigint(size); // fibn_2 = 0
    fibn = init_bigint(size);

    // Now we know that:
    //     n >= 2
    //     fibn_2 (aka fib(n-2)) = fib(0) = 0
    //     fibn_1 (aka fib(n-1)) = fib(1) = 1

    for (i = 2; i <= n; i++) {
        // fib(n) = fib(n-2) + fib(n-1)
        set_bigint(fibn_2, fibn);
        if (!add_bigint(fibn_1, fibn)) {
            printf("Overflow detected!\n");
            abort();
        }

        if (verbose)
            print_fib(i, fibn);

        // Now set fib(n-2) = fib(n-1), and fib(n-1) = fib(n)
        set_bigint(fibn_1, fibn_2);
        set_bigint(fibn, fibn_1);
    }

    /* Don't free fibn since we will be returning it. */
    free_bigint(fibn_2);
    free_bigint(fibn_1);

    return fibn;
}


void usage(const char *prog) {
    printf("usage: %s n size [-v]\n", prog);
    printf("\tPrints the n-th value in the Fibonacci sequence, using\n");
    printf("\ta size-quadword bigint representation for computations.\n");
    printf("\tfib(0) = 0, fib(1) = 1, fib(n) = fib(n-2) + fib(n-1).\n");
    printf("\n");
    printf("\t-v causes the program to print out all Fibonacci numbers\n");
    printf("\tbetween fib(0) and fib(n).\n");
}


/* This function takes two command-line arguments n and size, and finds the
 * n-th number in the Fibonacci sequence using a size-qword unsigned bigint.
 * The value is then printed.
 *
 * If either of the command-line arguments are invalid, an error is printed
 * out and the program terminates.
 */
int main(int argc, char **argv) {
    int n, raw_size;
    uint8_t size;
    bigint_t *fib_n;
   
    if (argc != 3 && argc != 4) {
        usage(argv[0]);
        return 1;
    }

    verbose = 0;
    if (argc == 4) {
        if (strcmp(argv[3], "-v") == 0) {
            verbose = 1;
        }
        else {
            usage(argv[0]);
            return 1;
        }
    }

    /* Parse the command-line arguments, and make sure they are valid. */

    n = atoi(argv[1]);
    if (n < 0) {
        printf("ERROR:  n must be >= 0\n");
        return 2;
    }

    raw_size = atoi(argv[2]);
    if (raw_size <= 0) {
        printf("ERROR:  size must be > 0\n");
        return 3;
    }
    size = (uint8_t) raw_size;

    /* Find the n-th value in the Fibonacci sequence, and print it. */

    printf("Finding fib(%d) using a %u-quadword bigint.\n", n, size);
    fib_n = big_fib(n, size);

    // If verbose output was on then fib(n) was already printed.
    if (!verbose)
        print_fib(n, fib_n);

    /* All done! */

    free_bigint(fib_n);
    return 0;
}

