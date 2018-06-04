/*! \file
 *
 * This file contains the definitions for the program counter of a branching
 * processor.  The program counter receives a signal from the branching unit
 * that tells it whether it should increment, or jump to a different location
 * in the program.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "branching_program_counter.h"
#include "branch_unit.h"
#include "instruction.h"

/*
 * Branching Program Counter
 *
 *  The Program Counter tells us where we
 *   are in a program.  It points to the
 *   address of the current instruction.
 *
 *  The Program Counter unit is capable of
 *   advancing the program counter to its
 *   next value OR loading a new value when
 *   a branch is specified.
 */

ProgramCounter * build_program_counter() {
    ProgramCounter *pc = malloc(sizeof(ProgramCounter));
    if (!pc) {
        fprintf(stderr, "Out of memory building a program counter!\n");
        exit(11);
    }
    memset(pc, 0, sizeof(ProgramCounter));
    return pc;
}


void free_program_counter(ProgramCounter *pc) {
    free(pc);
}


/*! Resets the program counter to 0. */
void reset(ProgramCounter *pc) {
    pin_set(pc->pc_pin, 0);
}


/*!
 * Advances the program counter by 1 byte.  This function does not use the
 * branching logic, since it is used for instruction decoding.
 */
void incrPC(ProgramCounter *pc) {
    busdata_t next_pc = pin_read(pc->pc_pin) + 1;
    pin_set(pc->pc_pin, next_pc);
}


/*!
 * Advances the program counter.  If the branching logic tells the processor
 * that it's time to jump somewhere else, the branch address is read and used.
 * Otherwise, the program counter is simply incremented.
 */
void nextPC(ProgramCounter *pc) {
    busdata_t next_pc;

    /*
     * increment the program counter, or load the program counter from the
     * branch address.
     */

    if (pin_read(pc->branch) == BRANCH)
        next_pc = pin_read(pc->branch_addr);
    else
        next_pc = pin_read(pc->pc_pin) + 1;

    /* Set the new value for the program counter. */
    pin_set(pc->pc_pin, next_pc);
}

