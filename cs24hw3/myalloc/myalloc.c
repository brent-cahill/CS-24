/*! \file
 * Implementation of a simple memory allocator.  The allocator manages a small
 * pool of memory, provides memory chunks on request, and reintegrates freed
 * memory back into the pool.
 *
 * Adapted from Andre DeHon's CS24 2004, 2006 material.
 * Copyright (C) California Institute of Technology, 2004-2010.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>

#include "myalloc.h"


/*!
 * These variables are used to specify the size and address of the memory pool
 * that the simple allocator works against.  The memory pool is allocated within
 * init_myalloc(), and then myalloc() and free() work against this pool of
 * memory that mem points to.
 */
int MEMORY_SIZE;
unsigned char *mem;


/*
 * We need to divide the memory into "blocks" that will hold the information
 * about the size and capacity of the block. The header will store the total
 * size of the block, and if we make this a signed integer, then we can
 * indicate whether it is full or empty using the sign. Thus, we know that
 * the header will be 4 bytes, as it holds an integer.
 */
#define HEADER_SIZE 4


/*!
 * This function initializes both the allocator state, and the memory pool.  It
 * must be called before myalloc() or myfree() will work at all.
 *
 * Note that we allocate the entire memory pool using malloc().  This is so we
 * can create different memory-pool sizes for testing.  Obviously, in a real
 * allocator, this memory pool would either be a fixed memory region, or the
 * allocator would request a memory region from the operating system (see the
 * C standard function sbrk(), for example).
 */

// This is a constant time function.
void init_myalloc() {

    /*
     * Allocate the entire memory pool, from which our simple allocator will
     * serve allocation requests.
     */
    mem = (unsigned char *) malloc(MEMORY_SIZE);
    if (mem == 0) {
        fprintf(stderr,
                "init_myalloc: could not get %d bytes from the system\n", MEMORY_SIZE);
        abort();
    }

    /*
     * Initial state of memory pool: one large block, with a 4 byte header
     * as we defined above. This block is free, therefore, we keep it
     * as a positive number.
     */
    *((int *) mem) = MEMORY_SIZE - HEADER_SIZE;
}


/*!
 * First-fit algorithm to allocate memory blocks. Linear.
 */
unsigned char *myalloc(int size) {

    /*
     * Begin by initializing the current block of memory that we will
     * use to look through the memory pool. Once a free block that is
     * large enough to fit the block is found, it is placed there. If
     * the entire block of memory has been iterated through and no
     * block of memory is found that is large enough, we return 0.
     * This has a time complexity of O(n), linear.
     */
    unsigned char *thisBlock = mem;
    int thisSpace = *((int *) thisBlock);
    int thisLoc = 0;

    /*
     * Iterate through the memory pool until a large enough memory
     * block is found. If the
     */
    while (thisSpace < size) {
        thisLoc += abs(thisSpace) + HEADER_SIZE;

        /*
         * Check to see if we are at the end of the memory pool.
         * If so, we set the current location equal to null, so that
         * we will return 0
         */
        if (thisLoc >= MEMORY_SIZE) {
            thisBlock = NULL;
            break;
        }
        // Otherwise, keep traversing
        else {
            thisBlock += abs(thisSpace) + HEADER_SIZE;
            thisSpace = *((int *) thisBlock);
        }
    }

    /*
     * If we have found a block location that has adequate size
     * then we will place our block in this location. First, we
     * ensure that thisBlock is not null:
     */
    if (thisBlock != NULL) {
        /*
         * Begin by creating a pointer that will refer to the
         * beginning of the data in which we intend to place the
         * block.
         */
        unsigned char *final = thisBlock + HEADER_SIZE;

        /*
         * The next thing we must do is check to see if there
         * is enough space left in the block to create another
         * block that can be used. In order for this to be the
         * case, it must be greater than HEADER_SIZE after
         * placing the block.
         */
        if (HEADER_SIZE <= thisSpace - size) {
            /* Initialize it as the size with a negative,
             * since it is taken.
             */
            *((int *) thisBlock) = -size;

            /*
             * Now create the second block that can be created
             * within the block
             */
            *((int *) (thisBlock + size + HEADER_SIZE)) =
                thisSpace - size - HEADER_SIZE;
        }
        /*
         * If the block cannot be broken down again, then
         * simply place the block into the current block
         * and mark as full.
         */
        else {
            *((int *) thisBlock) = -thisSpace;
        }

        // Return the final pointer to the memory
        return final;
    }

    /*
     * Otherwise, the current block was NULL, meaning that we
     * stop iterating and say that there is no free space left.
     */
    else {
        fprintf(stderr, "%s%d\n", "ERROR: unable to allocate memory block of size ", size);
        // cast 0 as an unsigned char *
        return (unsigned char *) 0;
    }
}


/*!
 * Free a previously allocated pointer.  Oldptr should be an address
 * returned by myalloc().
 * This frees up a specific block of memory. If any adjacent blocks
 * are also free, then merge the blocks. This function has a time
 * complexity of O(n), linear.
 */
void myfree(unsigned char *oldptr) {
    /*
     * Here we are writing our deallocator, which works as follows:
     * We must first subtract the size of the header from our given
     * pointer size, since right now we are pointing to the beginning
     * of the *data* not the beginning of the entire block, which
     * should include the header.
     */

    oldptr -= HEADER_SIZE;
    // initialize a variable called size, as before
    int size = *((int *) oldptr);
    // finally, set the cast oldptr = the unsigned size
    *((int *) oldptr) = abs(size);

    /*
     * Check to see if we can merge the newly freed block with
     * the block to its immediate right. This is realtively
     * simple, because we know the beginning of the next
     * block is the end of the current block.
     */

    // Initialize the variables to hold sizes and addresses
    unsigned char *newBlock = HEADER_SIZE + oldptr - size;

    if (*((int *) newBlock) > 0) {
        *((int *) oldptr) += HEADER_SIZE + *((int *) newBlock);
    }

    /*
     * Check to see if we can merge the newly freed block with
     * the block to its immediate left
     */

    // Initialize the variables to hold sizes and addresses
    unsigned char *thisBlock = mem;
    int thisSpace = *((int *) thisBlock);
    unsigned char *new_Block;
    int newSpace;
    int newLoc = abs(thisSpace) + HEADER_SIZE;

    while (newLoc < MEMORY_SIZE - HEADER_SIZE) {
        new_Block = thisBlock + HEADER_SIZE + abs(thisSpace);
        newSpace = *((int *) new_Block);

        // If the next block is free (positive), then merge them.
        if (newSpace > 0 && thisSpace > 0) {
            *((int *) thisBlock) += HEADER_SIZE + newSpace;
            break;
        }
        else {
            // move to the next block
            thisBlock = new_Block;
            thisSpace = newSpace;
            newLoc += abs(thisSpace) + HEADER_SIZE;
        }
    }
}

/*!
 * Clean up the allocator state.
 * All this really has to do is free the user memory pool. This function mostly
 * ensures that the test program doesn't leak memory, so it's easy to check
 * if the allocator does.
 */
void close_myalloc() {
    free(mem);
}
