/*
 * Test program implements a producer-consumer problem.
 * There are two producers that each produce Fibonacci
 * numbers and add the results to the buffer.  There
 * is a single consumer that takes the results from
 * the buffer, checks that the values are correct,
 * and prints them out.
 *
 *--------------------------------------------------------------------
 * Adapted from code for CS24 by Jason Hickey.
 * Copyright (C) 2003-2010, Caltech.  All rights reserved.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "sthread.h"
#include "semaphore.h"
#include "bounded_buffer.h"

/*
 * Queue for passing values between the producers and the consumer.
 */

#define DEFAULT_BUFFER_LENGTH 20

static BoundedBuffer *queue;


/*
 * Recursive Fibonacci.
 */
static int fib(int i) {
    if (i < 2)
        return 1;

    return fib(i - 1) + fib(i - 2);
}

/*
 * Producers generate Fibonacci numbers in a particular range, because we
 * need them to take a certain amount of time to allow for context switches
 * to occur.  The timer is configured to context-switch every 500us.  If we
 * generate Fibonacci numbers that take between 5us and 100us to generate,
 * threads can generate a handful of values before being preempted.  This
 * will exercise our bounded buffer, etc.
 */

#define MIN_FIB_USEC 5
#define MAX_FIB_USEC 100

static int MIN_FIB;
static int MAX_FIB;


static void producer(void *arg) {
    BufferElem elem;
    int i = MIN_FIB;

    elem.id = (intptr_t) arg;
    while (1) {
        /* Place the next computed Fibonacci result into the buffer */
        elem.arg = i;
        elem.val = fib(i);
        bounded_buffer_add(queue, &elem);
        i++;
        if (i > MAX_FIB)
            i = MIN_FIB;
    }
}


/*
 * Consumer prints them out.
 */
static void consumer(void *arg) {
    BufferElem elem;
    int i;

    /* Read the contents of the buffer, and print them */
    while (1) {
        bounded_buffer_take(queue, &elem);
        i = fib(elem.arg);
        printf("Result from producer %d: Fib %2d = %8d; should be %8d; %s\n",
               elem.id, elem.arg, elem.val, i,
               i == elem.val ? "matched" : "NO MATCH");
    }
}


/* Find the lowest Fibonacci number that takes at least the specified
 * amount of time to compute on this system.
 */
int find_fib_time(int i_start, int length_us) {
    int i = i_start;
    while (1) {
        struct timespec t_start, t_end;
        long diff;

        if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t_start)) {
            perror("get start time");
            exit(1);
        }

        (void) fib(i);

        if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t_end)) {
            perror("get end time");
            exit(1);
        }

        // Compute difference between end time and start time in us.
        diff = t_end.tv_nsec - t_start.tv_nsec;
        diff += (t_end.tv_sec - t_start.tv_sec) * 1000000000;
        diff /= 1000;

        if (diff >= length_us) {
            return i;
            break;
        }

        i++;
    }
}


/*
 * The main function starts the two producers and the consumer,
 * the starts the thread scheduler.
 */
int main(int argc, char **argv) {
    printf("Calibrating test.\n");

    /* Find the lowest Fibonacci number that takes at least MIN_FIB_USEC
     * to compute on this system.
     */
    MIN_FIB = find_fib_time(1, MIN_FIB_USEC);

    /* Find the lowest Fibonacci number that takes at least MAX_FIB_USEC
     * to compute on this system.
     */
    MAX_FIB = find_fib_time(MIN_FIB, MAX_FIB_USEC);

    printf("Computing Fibonacci numbers in the range [%d, %d].\n\n",
           MIN_FIB, MAX_FIB);

    sleep(1);  /* Pause for effect! */

    queue = new_bounded_buffer(DEFAULT_BUFFER_LENGTH);
    sthread_create(producer, (void *) 0);
    sthread_create(producer, (void *) 1);
    sthread_create(consumer, (void *) 0);

    /*
     * Start the thread scheduler.  By default, the timer is
     * not started.  Change the argument to 1 to start the timer.
     */
    sthread_start(1);
    return 0;
}

