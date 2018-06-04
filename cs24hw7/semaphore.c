/*
 * General implementation of semaphores.
 *
 *--------------------------------------------------------------------
 * Adapted from code for CS24 by Jason Hickey.
 * Copyright (C) 2003-2010, Caltech.  All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>

#include "sthread.h"
#include "semaphore.h"
#include "queue.h"
#include "glue.h"

/*
 * The semaphore data structure contains simply an integer i >= 0 and a queue
 * blocked that contains the blocked threads.
 */
struct _semaphore {
    int i;
    Queue *blocked;
};

/************************************************************************
 * Top-level semaphore implementation.
 */

/*
 * Allocate a new semaphore.  The initial value of the semaphore is
 * specified by the argument.
 */
Semaphore *new_semaphore(int init) {
    Semaphore *semp = (Semaphore *) malloc(sizeof(Semaphore));
    /* Assert it has been correctly allocated */
    assert(semp);

    /* Now ensure that init is indeed nonnegative, then initialize the value
     * i of the Semaphore to i.
     */
    assert(init >= 0);
    semp->i = init;

    /* Allocate a blocked queue, then initialize that blocked queue */
    semp->blocked = (Queue *) malloc(sizeof(Queue *));

    /* Assert it has been correctly allocated */
    assert(semp->blocked);

    /* Set the head and tail of the Queue to be NULL */
    semp->blocked->head = NULL;
    semp->blocked->tail = NULL;

    return semp;
}

/*
 * Decrement the semaphore.
 * This operation must be atomic, and it blocks iff the semaphore is zero.
 */
void semaphore_wait(Semaphore *semp) {
    /* Lock the system */
    __sthread_lock();

    /* Endure that the semaphore value is nonzero, so wait until we reach a
     * nonzero value.
     */
    while (semp->i == 0) {
    	/* Put sthread_current in the blocked queue, then block it */
    	queue_append(semp->blocked, sthread_current());
    	sthread_block();
    }
    /* We must now lock the system, since the scheduler leaves the system
     * unlocked when we wrote that function. We know that the scheduler is
     * called because the above statement sthread_block calls the scheduler via
     * __sthread_switch. We must have the system locked when we exit this
     * function because the functions to add and take in bounded_buffer require
     * the system to be locked, since they cannot call the lock and unlock
     * functions.
     * Finally, we decrement the semaphore.
	 */
    __sthread_lock();
    semp->i--;
}

/*
 * Increment the semaphore.
 * This operation must be atomic.
 */
void semaphore_signal(Semaphore *semp) {
    /* Make sure that the system is curerntly locked */
    assert(!__sthread_lock());

    /* Then we can increment the semaphore. */
    semp->i++;

    /* Now unblock the threads */
    if (semp->blocked->head) {
    	sthread_unblock(queue_take(semp->blocked));
    }

    /* Finally, we leave the function with the state in an unlocked state */
    __sthread_unlock();
}