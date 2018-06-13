#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

#include "bbrot.h"


/* Change #define to #undef for non-threadsafe increment in record_point(). */
#define THREADSAFE_INCR


/* These are all internal helper functions for the computation. */

int compute_bbrot_point(complex_t *c, uint32_t max_iters, int32_t bbrot_size,
                        uint32_t *array, complex_t *points);

void record_point_list(complex_t *points, uint32_t num_points,
                       int32_t bbrot_size, uint32_t *array);

void record_point(complex_t *c, int32_t bbrot_size, uint32_t *array);


/* Allocates an array of bbrot_size x bbrot_size uint32s, so that we can keep
 * track of each pixel's count.
 */
uint32_t * alloc_bbrot_array(int32_t bbrot_size) {
    assert(bbrot_size > 0);
    return malloc(bbrot_size * bbrot_size * sizeof(uint32_t));
}


/* This function computes a Buddhabrot image of size bbrot_size x bbrot_size,
 * by generating num_points points that have an escape-limit of max_iters.
 * Initial points are randomly generated (the random number generator is seeded
 * by this function); since a given point may not be included in the final
 * image, points that are discarded are not counted against the num_points
 * count.
 *
 * Arguments:
 *     num_points - the total number of starting points to use in the image.
 *
 *     max_iters - the maximum number of times to iterate the function before
 *         a point is considered to be "in" the Mandelbrot set.
 *
 *     bbrot_size - the dimension of one side of the image; the image is
 *         square, and is therefore of size bbrot_size x bbrot_size.
 *
 *     array - the array of pixel-counts that the image is generated into.
 */
void compute_bbrot(uint32_t num_points, uint32_t max_iters,
                   int32_t bbrot_size, uint32_t *array) {
    uint64_t seed;
    uint32_t i;

    /* For random number generation: */
    unsigned short xsubi[3];
    struct timeval time;

    assert(array != NULL);

    /* Generate a seed for the random number generator. */
    gettimeofday(&time, NULL);
    seed = time.tv_sec * 1000 + time.tv_usec / 1000;
    xsubi[0] = seed & 0xFFFF;
    xsubi[1] = (seed >> 16) & 0xFFFF;
    xsubi[2] = (seed >> 32) & 0xFFFF;

    /* This is the array of complex points generated along the trajectory
     * starting at point c.  We need to record them all, because we don't
     * know if the starting point is inside or outside the Mandelbrot set
     * until we have iterated it.
     */
    complex_t *points = malloc(sizeof(complex_t) * max_iters);

    i = 0;
    while (i < num_points) {
        complex_t c;

        /* Generate a random starting point. */
        c.real = (float) erand48(xsubi) * 3.0 - 2.0;
        c.imag = (float) erand48(xsubi) * 3.0 - 1.5;

        /* If we ended up using this point, increment our counter. */
        if (compute_bbrot_point(&c, max_iters, bbrot_size, array, points))
            i++;
    }

    free(points);
}


/* This function iterates the Mandelbrot fractal function Z_n+1 = Z_n ^ 2 + c
 * until either the point escapes or we hit the maximum iteration limit.  All
 * points Z_i are recorded along the way, because we don't know if the point
 * will escape or not until it actually does.
 *
 * If the point escapes then the list of intermediate points are recorded in
 * the result image using the record_point_list() function.
 *
 * Arguments:
 *     c = complex number to use in the fractal function Z_n+1 = Z_n ^ 2 + c
 *
 *     max_iters = number of iterations we must reach before we decide that
 *         the point c is in the Mandelbrot set
 *
 *     bbrot_size = the size of the image being computed; that is, the image
 *         is of dimension bbrot_size x bbrot_size.
 *
 *     array = the image data being computed; an array of 32-bit unsigned
 *         integers, of size bbrot_size x bbrot_size.
 *
 *     points = an array of complex numbers, of length max_iters, so that the
 *         intermediate points can be stored into this array.  The array is
 *         reused for each starting-point so that we don't have to continually
 *         free and reallocate the array.
 */
int compute_bbrot_point(complex_t *c, uint32_t max_iters, int32_t bbrot_size,
                        uint32_t *array, complex_t *points) {
    uint32_t num_iters = 0;
    complex_t z = { 0, 0 };

    /* Iterate the specified starting-point c until we either hit the
     * maximum number of iterations, or we discover that the point escapes
     * the set.  (A point is considered to have escaped the Mandelbrot set
     * if its magnitude is >= 2.  To improve performance, we compare the
     * magnitude-squared to 4.)
     */
    while (num_iters < max_iters && complex_magsq(&z) <= 4.0) {
        /* Z_n+1 = Z_n ^ 2 + c */
        complex_mul(&z, &z);
        complex_add(c, &z);

        /* Store the new point; we don't know if it escapes yet or not. */
        points[num_iters] = z;
        ++num_iters;
    }

    if (complex_magsq(&z) > 4.0) {
        /* Only record points that escape. */
        record_point_list(points, num_iters, bbrot_size, array);
        return 1;
    }

    /* We ended up not using this point c to render the image.
     * Tell the caller so they can try another one.
     */
    return 0;
}


/* Records an array of complex-number points into the result array.  This is
 * a simple wrapper to the record_point() function, which does the hard work.
 *
 * Arguments:
 *     points = the array of complex numbers holding the points to record
 *
 *     num_points = the number of points in the array to record
 *
 *     bbrot_size = the size of the image being computed; that is, the image
 *         is of dimension bbrot_size x bbrot_size.
 *
 *     array = the image data being computed; an array of 32-bit unsigned
 *         integers, of size bbrot_size x bbrot_size.
 */
void record_point_list(complex_t *points, uint32_t num_points,
                       int32_t bbrot_size, uint32_t *array) {
    uint32_t i;

    /* Get a little peek into how many points are normally generated.
    fprintf(stderr, " %u", num_points);
    */

    for (i = 0; i < num_points; i++)
        record_point(points + i, bbrot_size, array);
}


/* Records a single complex-number point into the result array.  This is done
 * by translating the complex-number point into an x and y coordinate based
 * on the size of the image, and then incrementing the value stored at that
 * (x, y) coordinate.
 *
 * If the THREADSAFE_INCR symbol is defined, the increment is performed using
 * a "lock incl" instruction, so that it is safe even in the context of
 * multiple concurrently-executing threads.
 *
 * Arguments:
 *     points = the array of complex numbers holding the points to record
 *
 *     num_points = the number of points in the array to record
 *
 *     bbrot_size = the size of the image being computed; that is, the image
 *         is of dimension bbrot_size x bbrot_size.
 *
 *     array = the image data being computed; an array of 32-bit unsigned
 *         integers, of size bbrot_size x bbrot_size.
 */    
void record_point(complex_t *c, int32_t bbrot_size, uint32_t *array) {
    /* Convert the complex number c into integer (x, y) image coordinates. */
    int32_t x_coord = (uint32_t) ((c->imag + 1.5) * (float) bbrot_size / 3.0);
    int32_t y_coord = (uint32_t) ((c->real + 2.0) * (float) bbrot_size / 3.0);

    if (x_coord < 0 || x_coord >= bbrot_size)
        return;
    if (y_coord < 0 || y_coord >= bbrot_size)
        return;

#ifdef THREADSAFE_INCR
    /* We need to increment the corresponding array-element like this:
     *     array[y_coord * bbrot_size + x_coord]++;
     * However, to provide thread-safety, we can simply use an "incl"
     * instruction with the "lock" prefix so that two threads will never
     * have overlapping increments.
     */ 
    asm("lock incl %0" : : "m" (array[y_coord * bbrot_size + x_coord]) );
#else
    /* We expect to not need the thread-safe increment, so just do it the
     * unsafe way.
     */
    array[y_coord * bbrot_size + x_coord]++;
#endif
}


/* Outputs the Buddhabrot image data as a Portable PixMap, since it's all ASCII
 * and not a binary format.  This function scales the image data to 0..255, and
 * then outputs the PPM image.
 */
void output_ppm_image(int32_t bbrot_size, uint32_t *array) {
    uint32_t maxval = 0;
    int32_t i;

    /* Scale the counts in the array to 0..255 */

    for (i = 0; i < bbrot_size * bbrot_size; i++) {
        if (array[i] > maxval)
            maxval = array[i];
    }

    /* fprintf(stderr, "Max value found = %u\n", maxval); */

    for (i = 0; i < bbrot_size * bbrot_size; i++) {
        uint32_t scaled = (array[i] * 512) / maxval;
        if (scaled > 255)
            scaled = 255;

        array[i] = scaled;
    }

    /* Output the data as a Portable PixMap image. */

    printf("P3 %d %d 255\n", bbrot_size, bbrot_size);
    for (i = 0; i < bbrot_size * bbrot_size; i++)
        printf("%u %u %u\n", array[i], array[i], array[i]);
}



