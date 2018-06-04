#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bigint.h"
#include "str.h"


typedef unsigned __int128 uint128_t;


/* Initializes a bigint of the specified size, with a value of 0. */
bigint_t * init_bigint(uint8_t size) {
    assert(size > 0);

    size_t num_bytes = sizeof(bigint_t) + size * sizeof(uint64_t);
    bigint_t *n = malloc(num_bytes);
    if (n != NULL) {
        memset(n, 0, num_bytes);
        n->size = size;
    }
    return n;
}

/* Releases the memory of the specified bigint. */
void free_bigint(bigint_t *n) {
    assert(n != NULL);
    size_t num_bytes = sizeof(bigint_t) + n->size * sizeof(uint64_t);
    memset(n, 0xc8, num_bytes);  // Overwrite with a sentry value for debugging
    free(n);
}

/* Heap-allocates a copy of the specified bigint n, returning a pointer to
 * the copy.
 */
bigint_t * copy_bigint(bigint_t *n) {
    bigint_t *copy;
    assert(n != NULL);

    copy = init_bigint(n->size);
    if (copy != NULL)
        set_bigint(n, copy);

    return copy;
}

/* Sets the value of the bigint dst to be a copy of the bigint src.  The two
 * bigints must be the same size.
 */
void set_bigint(bigint_t *src, bigint_t *dst) {
    assert(src != NULL);
    assert(dst != NULL);
    assert(src->size == dst->size);

    memcpy(dst->data, src->data, src->size * sizeof(uint64_t));
}

/* Sets the bigint dst to hold the 64-bit unsigned value in value. */
void set_bigint_value(uint64_t value, bigint_t *n) {
    assert(n != NULL);
    assert(n->size != 0);

    memset(n->data, 0, n->size * sizeof(uint64_t));
    n->data[0] = value;
}

/* Returns nonzero if the bigint n is zero, or zero if n != 0. */
int bigint_is_zero(bigint_t *n) {
    int i;

    assert(n != NULL);
    assert(n->size > 0);

    for (i = 0; i < n->size; i++)
        if (n->data[i] != 0)
            return 0;

    return 1;
}

/* Prints the bigint n to the console, with no trailing newline. */
void print_bigint(bigint_t *n) {
    bigint_t *p;
    str_t s;

    assert(n != NULL);
    assert(n->size > 0);

    /* We need to create a string-buffer to receive the digits of the bigint
     * into, because we generate them from least-significant digit up to
     * most-significant digit, and that means they would be out of order when
     * we printed them.  So, generate the digits, then reverse the string!
     */
    init_str(&s);

    /* Make a copy of the bigint since dividing changes the bigint. */
    p = copy_bigint(n);
    if (p == NULL) {
        printf("Couldn't make a copy of n to print!\n");
        abort();
    }

    /* Keep dividing the bigint by 10 to get the next digit to print,
     * until the bigint is zero.
     */
    do {
        uint64_t rem = divmod_bigint(p, 10);
        append_char(&s, '0' + rem);
    }
    while (!bigint_is_zero(p));

    /* Reverse the string's contents, then print it out. */
    reverse_str(&s);
    for (int i = 0, len = strlen(s.psz); s.psz[i] != 0; i++, len--) {
        if (i > 0 && len % 3 == 0)
            printf(",");

        printf("%c", s.psz[i]);
    }

    free_bigint(p);
    clear_str(&s);
}


/* Signature for the assembly language function. */
int __add_bigint(uint64_t *a, uint64_t *b, uint8_t size);

/* Adds the bigint a into bigint b.  Returns 1 if the add is successful,
 * or 0 if bigint b overflows in the process.
 */
int add_bigint(bigint_t *a, bigint_t *b) {
    /* Do a whole bunch of checks to make sure things are valid! */
    assert(a != NULL);
    assert(a->size > 0);
    assert(b != NULL);
    assert(b->size > 0);
    assert(a->size == b->size);

    return __add_bigint(a->data, b->data, a->size);
}

/* Divides the specified bigint n by the value in divisor.  The bigint n
 * receives the quotient, and the remainder is returned from the function.
 * Note that this function is only for dividing a bigint by a dword-size
 * value.
 */
uint64_t divmod_bigint(bigint_t *n, uint64_t divisor) {
    assert(n != NULL);
    assert(n->size > 0);
    int i = n->size;

    /* Since we have a multi-dword value for our dividend, we start at the
     * most significant dword, do the division, then any remainder gets
     * incorporated into the next-most-significant dword when we divide that.
     * And so forth.  The compiler can generate pretty efficient code for
     * this, using the x86-64 div instruction.
     *
     * NOTE:  uint128_t is not supported on 32-bit platforms!!!!!
     */
    uint128_t dividend = 0;
    uint64_t quotient, remainder = 0;
    do {
        i--;
        dividend = ((uint128_t) remainder << 64) | n->data[i];
        quotient = dividend / divisor;
        remainder = dividend % divisor;

        n->data[i] = quotient;
    }
    while (i != 0);

    return remainder;
}

