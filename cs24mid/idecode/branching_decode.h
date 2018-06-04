/*! \file
 *
 * This file contains the declarations for the instruction decoder of the
 * branching processor.  The instruction decoder extracts bits from the current
 * instruction and turns them into separate fields that go to various functional
 * units in the processor.
 */


#ifndef BRANCHING_DECODE_H
#define BRANCHING_DECODE_H


#include "bus.h"
#include "instruction_store.h"
#include "branching_program_counter.h"


/*!
 * The branching instruction decoder extracts bits from the instruction and
 * turns them into separate fields that go to various functional units in the
 * processor.
 */
typedef struct Decode {
    /* Inputs */

    pin input;       /*!< Instruction byte from the instruction store. */

    /* Outputs */

    pin cpuop;        /*!< The operation the CPU is to perform. */
    
    pin src1_addr;    /*!< The register for src1, if it is a register. */
    pin src1_const;   /*!< The constant for src1, if it is a constant. */
    pin src1_isreg;   /*!< A flag set to 1 if src1 is a register. */

    pin src2_addr;    /*!< The register for src2. */

    pin dst_addr;     /*!< The destination register. */
    pin dst_write;    /*!< A flag controlling whether dest.reg. is written. */
    
    pin branch_addr;  /*!< The branch address, for branch instructions. */
} Decode;


/* Documentation appears in branching_decode.c. */
Decode * build_decode();
void free_decode(Decode *d);

void fetch_and_decode(InstructionStore *is, Decode *d, ProgramCounter *pc);


#endif /* BRANCHING_DECODE_H */

