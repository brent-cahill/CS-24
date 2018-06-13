#include <stdint.h>

#include "complex.h"

uint32_t * alloc_bbrot_array(int32_t bbrot_size);

void compute_bbrot(uint32_t num_points, uint32_t max_iters,
                   int32_t bbrot_size, uint32_t *array);

void output_ppm_image(int32_t bbrot_size, uint32_t *array);

