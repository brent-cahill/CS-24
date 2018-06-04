#include <stdint.h>

/* This is the unsigned "big integer" bigint data-type. */
typedef struct bigint_t {
    /* Number of 64-bit dwords in the big integer */
    uint8_t size;

    /* The actual dwords that comprise the big integer */
    uint64_t data[];
} bigint_t;


/* Initializes a bigint of the specified size, with a value of 0. */
bigint_t * init_bigint(uint8_t size);

/* Releases the memory of the specified bigint. */
void free_bigint(bigint_t *n);

/* Heap-allocates a copy of the specified bigint n, returning a pointer to
 * the copy.
 */
bigint_t * copy_bigint(bigint_t *n);

/* Sets the value of the bigint dst to be a copy of the bigint src.  The two
 * bigints must be the same size.
 */
void set_bigint(bigint_t *src, bigint_t *dst);

/* Sets the bigint dst to hold the 64-bit unsigned value in value. */
void set_bigint_value(uint64_t value, bigint_t *n);

/* Returns nonzero if the bigint n is zero, or zero if n != 0. */
int bigint_is_zero(bigint_t *n);

/* Prints the bigint n to the console, with no trailing newline. */
void print_bigint(bigint_t *n);


/* Adds the bigint a into bigint b.  Returns 1 if the add is successful,
 * or 0 if bigint b overflows in the process.
 */
int add_bigint(bigint_t *a, bigint_t *b);

/* Divides the specified bigint n by the value in divisor.  The bigint n
 * receives the quotient, and the remainder is returned from the function.
 * Note that this function is only for dividing a bigint by a dword-size
 * value.
 */
uint64_t divmod_bigint(bigint_t *n, uint64_t divisor);

