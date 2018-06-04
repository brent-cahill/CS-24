/*! \file
 *
 * This file contains declarations for the instruction store, which holds the
 * sequence of instructions that the processor executes.
 */


#ifndef INSTRUCTION_STORE_H
#define INSTRUCTION_STORE_H


#include "bus.h"


/*
 * Instruction Store
 *
 * The Instruction Store is a memory that holds the instructions that
 * describe the computation we wish to perform.
 *
 * The Instruction Store can be loaded from a file, and then will return
 * the value associated with each address.
 */

#define INSTRUCTION_STORE_DEPTH 200

typedef struct InstructionStore {
    pin address_input;   /*!< The address to fetch the instruction from. */
    pin output;          /*!< The instruction at the specified address. */

    /*! These are the actual instructions themselves. */
    uint8_t imemory[INSTRUCTION_STORE_DEPTH];
} InstructionStore;


InstructionStore *build_instruction_store();
void free_instruction_store(InstructionStore *is);

void ifetch(InstructionStore *is);

void load_instruction_store_from_fd(FILE *fd, InstructionStore *is);


#endif /* INSTRUCTION_STORE_H */


