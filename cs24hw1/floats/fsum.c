#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "ffunc.h"


/* This function takes an array of single-precision floating point values,
 * and computes a sum in the order of the inputs.  Very simple.
 */
float fsum(FloatArray *floats) {
    float sum = 0;
    int i;

    for (i = 0; i < floats->count; i++)
        sum += floats->values[i];

    return sum;
}


float my_fsum(FloatArray *floats, int b, int e) {
    // final sum
    float s;
    // iterator value and middle index value
    int i, m;

    // Base case, naive sum
    if (e - b <= 2){
        s = floats->values[b];
        for (i = b + 1; i < e; i++){
            s += floats->values[i];
        }
    }
    // Otherwise, recursively pairwise sum
    else{
        // Get integer division value for middle
        m = (int) ((b + e) / 2.0);
        // Call recursive sum
        s = my_fsum(floats, b, m) + my_fsum(floats, m, e);
    }
    return s;
}


int main() {
    FloatArray floats;
    float sum1, sum2, sum3, my_sum;

    load_floats(stdin, &floats);
    printf("Loaded %d floats from stdin.\n", floats.count);

    /* Compute a sum, in the order of input. */
    sum1 = fsum(&floats);

    /* Use my_fsum() to compute a sum of the values.  Ideally, your
     * summation function won't be affected by the order of the input floats.
     */
    my_sum = my_fsum(&floats, 0, floats.count);

    /* Compute a sum, in order of increasing magnitude. */
    sort_incmag(&floats);
    sum2 = fsum(&floats);

    /* Compute a sum, in order of decreasing magnitude. */
    sort_decmag(&floats);
    sum3 = fsum(&floats);

    /* %e prints the floating-point value in full precision,
     * using scientific notation.
     */
    printf("Sum computed in order of input:  %e\n", sum1);
    printf("Sum computed in order of increasing magnitude:  %e\n", sum2);
    printf("Sum computed in order of decreasing magnitude:  %e\n", sum3);

    
    printf("My sum:  %e\n", my_sum);

    return 0;
}

