#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <pthread.h>

#include "bbrot.h"


/* This struct is used when running with 2 or more threads. */
typedef struct bbrot_args {
    uint32_t num_points;

    uint32_t max_iters;

    int32_t bbrot_size;

    uint32_t *array;
} bbrot_args;


void *bbrot_thread(void *);


int main(int argc, char **argv) {
    int32_t bbrot_size;
    uint32_t num_points, max_iters;
    uint32_t *array;

    uint8_t num_threads, i;

    if (argc != 5) {
        printf("usage: %s size num_points max_iters num_threads\n\n", argv[0]);
        printf("\tsize is the dimension of the image to generate;\n"
               "\ta size x size image will be generated\n\n");
        printf("\tmax_iters is the maximum number of iterations to\n"
               "\tperform before considering a point to be \"inside\"\n"
               "\tthe Mandelbrot set\n\n");
        printf("\tnum_threads is the number of threads to use\n\n");
        return 1;
    }

    bbrot_size = atoi(argv[1]);
    num_points = atoi(argv[2]);
    max_iters = atoi(argv[3]);
    num_threads = atoi(argv[4]);

    fprintf(stderr,
        "Computing %dx%d Buddhabrot image from %u starting points and a\n"
        "max-iteration limit of %u.\n", bbrot_size, bbrot_size, num_points,
        max_iters);

    array = alloc_bbrot_array(bbrot_size);

    if (num_threads > 1) {
        /* Spin up the requested number of threads to compute Buddhabrot. */

        bbrot_args args = {
            num_points / num_threads,
            max_iters,
            bbrot_size,
            array
        };

        pthread_t *thread_ids = malloc(sizeof(pthread_t) * num_threads);
        for (i = 0; i < num_threads; i++)
            pthread_create(thread_ids + i, NULL, bbrot_thread, &args);

        /* Wait for all the threads to terminate. */
        for (i = 0; i < num_threads; i++)
            pthread_join(thread_ids[i], NULL);

        free(thread_ids);
    }
    else {
        /* Just one thread of execution - just call the function directly. */
        compute_bbrot(num_points, max_iters, bbrot_size, array);
    }

    output_ppm_image(bbrot_size, array);

    return 0;
}


/* A simple thread-function that wraps the call to compute_bbrot(). */
void *bbrot_thread(void *a) {
    bbrot_args *args = (bbrot_args *) a;

    fprintf(stderr, "Thread started to compute %u points.\n", args->num_points);
    compute_bbrot(args->num_points, args->max_iters,
                  args->bbrot_size, args->array);

    return NULL;
}



