#ifndef QUEUE_H
#define QUEUE_H

#include "sthread.h"

/*!
 * A queue of threads.  This type is used to keep track of threads in
 * various states within the user-space threaading library.
 */
typedef struct _queue {
    Thread *head;  /*!< The first thread in the queue. */
    Thread *tail;  /*!< The last thread in the queue. */
} Queue;


int queue_empty(Queue *queuep);
void queue_append(Queue *queuep, Thread *threadp);
void queue_add(Thread *threadp);
Thread *queue_take(Queue *queuep);
void queue_remove(Queue *queuep, Thread *threadp);


#endif /* QUEUE_H */

