/*!
 * Simple thread API.
 *
 * ----------------------------------------------------------------
 *
 * @begin[license]
 * Copyright (C) 2003 Jason Hickey, Caltech
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Author: Jason Hickey
 * @end[license]
 */
#ifndef _STHREAD_H
#define _STHREAD_H


/*!
 * The structure of the thread's machine-context is opaque to the C code.
 * Thread structs reference the context by a ThreadContext *, i.e. a void *
 * (untyped pointer).
 */
typedef void ThreadContext;


/*!
 * This enumeration defines all valid states of threads in the library.
 * A thread can be running, ready, blocked, or finished.
 */
typedef enum {
    /*!
     * The thread is currently running on the CPU.  Only one thread should be
     * in this state.
     */
    ThreadRunning,

    /*!
     * The thread is ready to run, but doesn't have access to the CPU yet.  The
     * thread will be kept in the ready-queue.
     */
    ThreadReady,

    /*!
     * The thread is blocked and currently unable to progress, so it is not
     * scheduled on the CPU.  Blocked threads are kept in the blocked-queue.
     */
    ThreadBlocked,

    /*!
     * The thread's function has returned, and therefore the thread is ready to
     * be permanently removed from the scheduling mechanism and deallocated.
     */
    ThreadFinished
} ThreadState;


/*! All details for recording the state of a thread. */
typedef struct _thread {
    /*! State of the thread. */
    ThreadState state;

    /*!
     * The start of the memory region being used for the thread's stack
     * and machine context.
     */
    void *memory;

    /*!
     * The machine context itself.  This will be some address within the
     * memory region referenced by the previous field.
     */
    ThreadContext *context;

    /*! The threads are linked in a doubly-linked list. */
    struct _thread *prev;
    struct _thread *next;
} Thread;


/*! The function that is passed to a thread must have this signature. */
typedef void (*ThreadFunction)(void *arg);


/*
 * Thread operations.
 */
Thread *sthread_create(ThreadFunction f, void *arg);
void sthread_yield(void);
void sthread_block(void);
void sthread_unblock(Thread *threadp);


/*
 * The start function should be called *once* in
 * the main() function of your program.  This function
 * never returns.
 */
void sthread_start(void);


#endif /* _STHREAD_H */

