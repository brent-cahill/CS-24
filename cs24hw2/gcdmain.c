#include <stdio.h>
#include <stdlib.h>

int gcd(int x, int y);

int main(int argc, char const *argv[]) {
    int num_1, num_2;
    int x, y; // Bigger and smaller, respectively

    if (argc != 3) {
        // != 2 arguments
        fprintf(stderr, "%s\n", "Error: Program requires 2 arguments.");
        exit(EXIT_FAILURE);
    }
    else {
        num_1 = atoi(argv[1]);
        num_2 = atoi(argv[2]);

        if (num_1 < 0 || num_2 < 0) {
            // Args must be nonnegative
            fprintf(stderr, "%s\n", "Error: Program requires nonnegative arguments.");
            exit(EXIT_FAILURE);
        }
        if (num_1 > num_2) {
            x = num_1;
            y = num_2;
        }
        else {
            x = num_2;
            y = num_1;
        }
        printf("GCD(%d, %d) = %d\n", num_1, num_2, gcd(x, y));
        exit(EXIT_SUCCESS);
    }
}
