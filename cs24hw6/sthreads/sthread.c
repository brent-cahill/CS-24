/*
 * The simple thread scheduler.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "sthread.h"
#include "queue.h"


/*! The default stack size of threads, 1MiB of stack space. */
#define DEFAULT_STACKSIZE       (1 << 20)


/************************************************************************
 * Interface to the assembly.
 */


/*! This function must be called to start thread processing. */
void __sthread_start(void);


/*!
 * The is the entry point into the scheduler, to be called
 * whenever a thread action is required.
 */
void __sthread_switch(void);

/*!
 * Initialize the context for a new thread.
 *
 * The stackp pointer should point to the *end* of the area allocated for the
 * thread's stack.  (Don't forget that x86 stacks grow downward in memory.)
 *
 * The function f is initially called with the argument given.  When f
 * returns, __sthread_finish() is automatically called by the threading
 * library to ensure that the thread is cleaned up and deallocated properly.
 */
ThreadContext *__sthread_initialize_context(void *stackp, ThreadFunction f,
                                            void *arg);

/************************************************************************
 * Internal helper functions.
 */

/*
 * The initial thread context is initialized such that this function
 * will be called automatically when a thread's function returns.
 */
void __sthread_finish(void);

/*
 * This function deallocates the memory associated with a thread
 * when it terminates.
 */
void __sthread_delete(Thread *threadp);


/************************************************************************
 * Global variables and scheduler-level queue operations.
 */

/*!
 * The thread that is currently running.
 *
 * Invariant: during normal operation, there is exactly one thread in
 * the ThreadRunning state, and this variable points to that thread.
 *
 * Note that at start-up of the threading library, this will be NULL.
 */
static Thread *current;

/*!
 * The queue of ready threads.  Invariant:  All threads in the ready queue
 * are in the state ThreadReady.
 */
static Queue ready_queue;

/*!
 * The queue of blocked threads.  Invariant:  All threads in the blocked
 * queue are in the state ThreadBlocked.
 */
static Queue blocked_queue;


/*!
 * Add a thread to the appropriate scheduling queue, based on its state.
 */
static void enqueue_thread(Thread *threadp) {
    assert(threadp != NULL);

    switch(threadp->state) {
    case ThreadReady:
        queue_append(&ready_queue, threadp);
        break;
    case ThreadBlocked:
        queue_append(&blocked_queue, threadp);
        break;
    default:
        fprintf(stderr, "Thread state has been corrupted: %d\n",
                threadp->state);
        exit(1);
    }
}

/************************************************************************
 * Scheduler.
 */

/*
 * The scheduler is called with the context of the current thread,
 * or NULL when the scheduler is first started.
 *
 * The general operation of this function is:
 *   1.  Save the context argument into the current thread.
 *   2.  Either queue up or deallocate the current thread,
 *       based on its state.
 *   3.  Select a new "ready" thread to run, and set the "current"
 *       variable to that thread.
 *        - If no "ready" thread is available, examine the system
 *          state to handle this situation properly.
 *   4.  Return the context of the thread to run, so that a context-
 *       switch will be performed to start running the next thread.
 *
 * This function is global because it needs to be called from the assembly.
 */
ThreadContext *__sthread_scheduler(ThreadContext *context) {

		/* Ensure this is not the first call to scheduler */
    if (context) {
     	/* save ^ */
    	current->context = context;

     	switch (current->state) {
	     	case ThreadRunning:
	    		current->state = ThreadReady;
	    		/* Fall through */

	    	case ThreadBlocked:
	    		enqueue_thread(current);
	    		break;

	    	case ThreadFinished:
	    		__sthread_delete(current);
	    		break;

	    	case ThreadReady:
	    		fprintf(stderr, "%s\n", "Error: Scheduler called with ready thread");

	    	default:
	    		assert(0);
	    		break;
	    }
    }

    /* New thread, and update the new current variable */
    current = queue_take(&ready_queue);

    /* In the case that there is no current ready thread */
    if (current == NULL) {
    	/* If there are no blocked threads, exit */
    	if (queue_empty(&blocked_queue)) {
    		printf("%s\n", "All threads complete.");
    		exit(0);
    	}

    	/* Otherwise, we ented a deadlock, and exit. */
    	else {
    		fprintf(stderr, "%s\n", "Program has entered deadlock.");
    		exit(1);
    	}
    }

    current->state = ThreadRunning;

    /* Return the next thread to resume executing. */
    return current->context;
}


/************************************************************************
 * Thread operations.
 */

/*
 * Start the scheduler.
 */
void sthread_start(void)
{
    __sthread_start();
}

/*
 * Create a new thread.
 *
 * This function allocates a new context, and a new Thread
 * structure, and it adds the thread to the Ready queue.
 */
Thread * sthread_create(void (*f)(void *arg), void *arg) {
    /* Begin by allocating a new ans Thread, then assert the ensure proper
     * allocation. */
    Thread *ans = (Thread *) malloc(sizeof(Thread));
    assert(ans);
    ans->state = ThreadReady;

    /* Now allocate a stack */
    void *stackp = malloc(DEFAULT_STACKSIZE);
    assert(stackp);
    ans->memory = stackp;

    /* Get the top of the stack for the following initialization */
    void *stack_top = (void *) (((char *) stackp) + DEFAULT_STACKSIZE);

    /* Set the new context */
    ans->context = __sthread_initialize_context(stack_top, f, arg);

    /* Put the new thread on the queue */
    enqueue_thread(ans);

    return ans;
}


/*
 * This function is called automatically when a thread's function returns,
 * so that the thread can be marked as "finished" and can then be reclaimed.
 * The scheduler will call __sthread_delete() on the thread before scheduling
 * a new thread for execution.
 *
 * This function is global because it needs to be referenced from assembly.
 */
void __sthread_finish(void) {
    printf("Thread %p has finished executing.\n", (void *) current);
    current->state = ThreadFinished;
    __sthread_switch();
}


/*!
 * This function is used by the scheduler to release the memory used by the
 * specified thread.  The function deletes the memory used for the thread's
 * context, as well as the memory for the Thread struct.
 */
void __sthread_delete(Thread *threadp) {
    /* Free context mem */
    free(threadp->memory);

    /* Free memory from entire Thread */
    free(threadp);
}


/*!
 * Yield, so that another thread can run.  This is easy: just
 * call the scheduler, and it will pick a new thread to run.
 */
void sthread_yield() {
    __sthread_switch();
}


/*!
 * Block the current thread.  Set the state of the current thread
 * to Blocked, and call the scheduler.
 */
void sthread_block() {
    current->state = ThreadBlocked;
    __sthread_switch();
}


/*!
 * Unblock a thread that is blocked.  The thread is placed on
 * the ready queue.
 */
void sthread_unblock(Thread *threadp) {

    /* Make sure the thread was blocked */
    assert(threadp->state == ThreadBlocked);

    /* Remove from the blocked queue */
    queue_remove(&blocked_queue, threadp);

    /* Re-queue it */
    threadp->state = ThreadReady;
    enqueue_thread(threadp);
}

