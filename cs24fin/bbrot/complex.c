#include <assert.h>
#include <stdlib.h>

#include "complex.h"

/* Add the complex numbers src and dst, storing the result into dst.  It is
 * fine if src and dst are the same complex-number struct; the answer will
 * still be correct.
 */
void complex_add(complex_t *src, complex_t *dst) {
    assert(src != NULL);
    assert(dst != NULL);

    dst->real += src->real;
    dst->imag += src->imag;
}

/* Multiply the complex numbers src and dst, storing the result into dst.
 * It is fine if src and dst are the same complex-number struct; the answer
 * will still be correct.
 */
void complex_mul(complex_t *src, complex_t *dst) {
    float re, im;

    assert(src != NULL);
    assert(dst != NULL);

    re = src->real * dst->real - src->imag * dst->imag;
    im = src->real * dst->imag + src->imag * dst->real;

    dst->real = re;
    dst->imag = im;
}

/* Compute the magnitude-squared of the complex number c. */
float complex_magsq(complex_t *c) {
    assert(c != NULL);
    return c->real * c->real + c->imag * c->imag;
}

