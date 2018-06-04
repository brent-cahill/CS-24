/*! \file
 *
 * This file contains definitions for an Arithmetic/Logic Unit of an
 * emulated processor.
 */


#include <stdio.h>
#include <stdlib.h>   /* malloc(), free() */
#include <string.h>   /* memset() */

#include "alu.h"
#include "instruction.h"


/*!
 * This function dynamically allocates and initializes the state for a new ALU
 * instance.  If allocation fails, the program is terminated.
 */
ALU * build_alu() {
    /* Try to allocate the ALU struct.  If this fails, report error then exit. */
    ALU *alu = malloc(sizeof(ALU));
    if (!alu) {
        fprintf(stderr, "Out of memory building an ALU!\n");
        exit(11);
    }

    /* Initialize all values in the ALU struct to 0. */
    memset(alu, 0, sizeof(ALU));
    return alu;
}


/*! This function frees the dynamically allocated ALU instance. */
void free_alu(ALU *alu) {
    free(alu);
}


/*!
 * This function implements the logic of the ALU.  It reads the inputs and
 * opcode, then sets the output accordingly.  Note that if the ALU does not
 * recognize the opcode, it should simply produce a zero result.
 */
void alu_eval(ALU *alu) {
    busdata_t A, B, aluop;
    busdata_t result;

    int set_status;

    A = pin_read(alu->in1);

    if (pin_read(alu->in2_reg))
        B = pin_read(alu->in2);
    else
        B = pin_read(alu->const2);

    aluop = pin_read(alu->op);

    result = 0;

    set_status = 1;
    switch (aluop) {
            
        case OP_MOV:     /* Simple data movement. */
            result = B;
            set_status = 0;
            break;

        case OP_ADD:     /* Addition. */
            result = A + B;
            break;
            
        case OP_SUB:     /* Subtraction. */
            result = A - B;
            break;

        case OP_NEG:     /* Twos-complement negation */
            result = (uint32_t) (-(int32_t) A);
            break;

        case OP_INC:      /* Increment. */
            result = A + 1;
            break;

        case OP_DEC:      /* Increment. */
            result = A - 1;
            break;

            
        case OP_INV:      /* Ones-complement negation, a.k.a. inversion */
            result = ~A;
            break;
            
        case OP_AND:      /* Bitwise AND */
            result = A & B;
            break;
            
        case OP_OR:       /* Bitwise OR */
            result = A | B;
            break;
            
        case OP_XOR:      /* Bitwise exclusive-OR */
            result = A ^ B;
            break;
            
        case OP_SHL:      /* Shift-left instructions */
            result = A << 1;
            break;
            
        case OP_SHR:      /* Logical shift-right */
            /* Since A is already unsigned, C will
             * automatically perform logical shift-right.
             */
            result = A >> 1;
            break;
            
        default:          /* Unrecognized opcode; don't do anything. */
            result = 0;
            set_status = 0;
            break;
    }

    pin_set(alu->out, result);

    /* Set the status output to be 1 if the result is 0, or 0 otherwise. */
    if (set_status)
        pin_set(alu->status, (result == 0 ? 1 : 0));
}

