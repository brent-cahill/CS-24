typedef struct complex_t {
    float real, imag;
} complex_t;

void complex_add(complex_t *src, complex_t *dst);
void complex_mul(complex_t *src, complex_t *dst);
float complex_magsq(complex_t *c);

