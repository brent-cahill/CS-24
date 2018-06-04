#include <assert.h>
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
void queue_append(Queue *queuep, Thread *threadp)
{
    if(queuep->head == NULL) {
        threadp->prev = NULL;
        threadp->next = NULL;
        queuep->head = threadp;
        queuep->tail = threadp;
    }
    else {
        queuep->tail->next = threadp;
        threadp->prev = queuep->tail;
        threadp->next = NULL;
        queuep->tail = threadp;
    }
}


/*!
 * Get the first thread from the queue.  Returns NULL if the queue is empty.
 */
Thread *queue_take(Queue *queuep) {
    Thread *threadp;

    assert(queuep != NULL);
    
    /* Return NULL if the queue is empty */
    if(queuep->head == NULL)
        return NULL;

    /* Go to the final element */
    threadp = queuep->head;
    if(threadp == queuep->tail) {
        queuep->head = NULL;
        queuep->tail = NULL;
    }
    else {
        threadp->next->prev = NULL;
        queuep->head = threadp->next;
    }
    return threadp;
}


/*! Remove a thread from a queue. */
void queue_remove(Queue *queuep, Thread *threadp) {
    assert(queuep != NULL);
    assert(threadp != NULL);

    /* Unlink */
    if(threadp->prev != NULL)
        threadp->prev->next = threadp->next;
    if(threadp->next != NULL)
        threadp->next->prev = threadp->prev;

    /* Reset head and tail pointers */
    if(queuep->head == threadp)
        queuep->head = threadp->next;
    if(queuep->tail == threadp)
        queuep->tail = threadp->prev;
}

