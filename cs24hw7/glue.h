/* C declarations for the functions in the glue code.
 *--------------------------------------------------------------------
 * Adapted from code for CS24 by Jason Hickey.
 * Copyright (C) 2003-2010, Caltech.  All rights reserved.
 */
#ifndef _GLUE_H
#define _GLUE_H


/* Lock the scheduler.  This function is used to ensure
 * mutual exclusion in the scheduler.  The function returns
 * 1 if the lock was successful, otherwise it returns 0.
 */
extern int __sthread_lock(void);

/* Unlock the scheduler. */
extern void __sthread_unlock(void);


/* The entry-point into the context-switch mechanism. */
void __sthread_switch(void);

#endif /* _GLUE_H */

