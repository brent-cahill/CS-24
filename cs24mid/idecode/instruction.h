/*! \file
 *
 * This file contains all constants associated with instructions in the simple
 * processor.
 */


/**** REGISTER-FILE CONSTANTS ****/


/*! Specifies the number of address bits for the register file. */
#define REGISTER_BITS 3

/*!
 * Using the number of register-bits, this expression is the total number of
 * registers in the register file.
 */
#define NUM_REGISTERS (1 << REGISTER_BITS)


/**** CPU OPCODES ****/

/* Zero-argument instructions: */

#define OP_DONE  0x00  /*!< The opcode to stop execution. */

/* One-argument instructions: */

#define OP_INC   0x01  /*!< The opcode to increment a register. */
#define OP_DEC   0x02  /*!< The opcode to decrement a register. */
#define OP_NEG   0x03  /*!< The opcode to perform twos-complement negation. */
#define OP_INV   0x04  /*!< The opcode to perform ones-complement inversion. */

#define OP_SHL   0x05  /*!< The opcode for shift-left. */
#define OP_SHR   0x06  /*!< The opcode for shift-right. */

/* Two-argument instructions: */

#define OP_MOV   0x08  /*!< The opcode for simple move operation. */
#define OP_ADD   0x09  /*!< The opcode for addition. */
#define OP_SUB   0x0A  /*!< The opcode for subtraction. */
#define OP_AND   0x0C  /*!< The opcode for bitwise AND. */
#define OP_OR    0x0D  /*!< The opcode for bitwise OR.  */
#define OP_XOR   0x0E  /*!< The opcode for bitwise XOR. */

/* Branching instructions: */

#define OP_BRA   0x07  /*!< The opcode for unconditional branch. */
#define OP_BRZ   0x0B  /*!< The opcode for branch-if-not-zero. */
#define OP_BNZ   0x0F  /*!< The opcode for branch-if-not-zero. */



