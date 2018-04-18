/* This file contains x86-64 assembly-language implementations of three
 * basic, very common math operations.
 */

        .text

/*====================================================================
 * int f1(int x, int y)
 */
.globl f1
f1:
        movl    %edi, %edx
        movl    %esi, %eax
        cmpl    %edx, %eax
        cmovg   %edx, %eax
        ret


/*====================================================================
 * int f2(int x)
 */
.globl f2
f2:
        movl    %edi, %eax
        movl    %eax, %edx
        sarl    $31, %edx
        xorl    %edx, %eax
        subl    %edx, %eax
        ret


/*====================================================================
 * int f3(int x)
 */
.globl f3
f3:
        movl    %edi, %edx
        movl    %edx, %eax
        sarl    $31, %eax
        testl   %edx, %edx
        movl    $1, %edx
        cmovg   %edx, %eax
        ret

