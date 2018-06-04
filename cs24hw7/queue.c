#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"


/*!
 * Returns true (1) if the specified queue is empty.  Otherwise, returns
 * false (0).
 */
int queue_empty(Queue *queuep) {
    assert(queuep != NULL);
    return (queuep->head == NULL);
}


/*!
 * Add the process to the head of the queue.  If the queue is empty, add the
 * singleton element.  Otherwise, add the element as the tail.
 */
void queue_append(Queue *queuep, Thread *threadp) {
    QueueNode *nodep = (QueueNode *) malloc(sizeof(QueueNode));
    if (nodep == NULL) {
        fprintf(stderr, "Couldn't allocate QueueNode\n");
        abort();
    }

    nodep->threadp = threadp;

    if(queuep->head == NULL) {
        nodep->prev = NULL;
        nodep->next = NULL;
        queuep->head = nodep;
        queuep->tail = nodep;
    }
    else {
        queuep->tail->next = nodep;
        nodep->prev = queuep->tail;
        nodep->next = NULL;
        queuep->tail = nodep;
    }
}


/*!
 * Get the first thread from the queue.  Returns NULL if the queue is empty.
 */
Thread *queue_take(Queue *queuep) {
    QueueNode *nodep;
    Thread *threadp;

    assert(queuep != NULL);
    
    /* Return NULL if the queue is empty */
    if(queuep->head == NULL)
        return NULL;

    /* Go to the final element */
    nodep = queuep->head;
    if(nodep == queuep->tail) {
        queuep->head = NULL;
        queuep->tail = NULL;
    }
    else {
        nodep->next->prev = NULL;
        queuep->head = nodep->next;
    }

    threadp = nodep->threadp;
    free(nodep);
    return threadp;
}


/*!
 * Remove a thread from a queue.
 *
 * Returns 1 if the thread was found in the queue, or 0 if the thread was not
 * found in the queue.
 *
 * NOTE:  DO NOT use this operation in an assertion, e.g.
 *            assert(queue_remove(somequeuep, somethreadp));
 *        Assertions may be compiled out of a program, and if they are, any
 *        side-effects in the assertion's test will also be compiled out.
 */
int queue_remove(Queue *queuep, Thread *threadp) {
    QueueNode *nodep;

    assert(queuep != NULL);
    assert(threadp != NULL);

    /* Find the node in the queue with the specified thread. */
    nodep = queuep->head;
    while (nodep != NULL && nodep->threadp != threadp)
        nodep = nodep->next;

    if (!nodep)
        return 0;

    /* Found the node; unlink it from the list. */
    if (nodep->prev != NULL)
        nodep->prev->next = nodep->next;
    if (nodep->next != NULL)
        nodep->next->prev = nodep->prev;

    /* Reset head and tail pointers */
    if(queuep->head == nodep)
        queuep->head = nodep->next;
    if(queuep->tail == nodep)
        queuep->tail = nodep->prev;

    /* Delete the node. */
    free(nodep);

    /* We removed a node so return 1. */
    return 1;
}