#include <stdio.h>
#include <stdlib.h>
#include "fact.h"

int main(int argc, char const *argv[]) {
    int arg_;

    if (argc != 2) {
        // != 1 argument
        fprintf(stderr, "%s\n", "Error: Program requires one argument.");
        exit(EXIT_FAILURE);
    }
    else {
        arg_ = atoi(argv[1]);
        if (arg_ < 0) {
            // Negative argument passed
            fprintf(stderr, "%s\n", "Error: Program requires nonnegative argument.");
            exit(EXIT_FAILURE);
        }
        printf("%d! = %d\n", arg_, fact(arg_));
        exit(EXIT_SUCCESS);
    }
}
