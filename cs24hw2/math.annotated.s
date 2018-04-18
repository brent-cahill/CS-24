/* This file contains x86-64 assembly-language implementations of three
 * basic, very common math operations.
 *
 * The Naive implementations of these statements would utilize jump
 * statements, which some of these avoid. This is important because
 * jump statements can slow down the CPU by requiring branching and
 * predictions. If the predictions are correct, great, but if they
 * are wrong, then the computer just wasted time predicting values
 * that are irrelevant. This is avoided by using predicated
 * instructions, an alternative to conditional branch instructions.
 * This means that the CPU can use any predictions going forward.
 * Nothing that the computer does will ned to be undone.
 */

        .text

/*====================================================================
 * int f1(int x, int y)
 * 
 * This function returns the smaller of the two integers passed to it.
 */
.globl f1
f1:
        movl    %edi, %edx  # move x to %edx
        movl    %esi, %eax  # move y to %eax
        cmpl    %edx, %eax  # compare y to x
        cmovg   %edx, %eax  # if y > x, then y = x
        ret                 # return y


/*====================================================================
 * int f2(int x)
 *
 * This function clearly returns the absolute value of the passed integer.
 * It does this by copying the input to %edx, then arithmetically shifting
 * right until we can see the sign. It checks the sign, then sets %eax to be
 * equal to itself if the sign was positive, otherwise it sets %eax to be
 * NOT x. It then leaves %eax alone if it was >=0, otherwise it will get the
 * 2s complement, giving us -x in %eax. It then returns the absolute value
 * stored in %eax.
 */
.globl f2
f2:
        movl    %edi, %eax  # move x to %eax
        movl    %eax, %edx  # copy x to %edx
        sarl    $31, %edx   # arithmetic shift right 31 times
                            # (to get sign), store in %edx
        xorl    %edx, %eax  # set x = x iff x >= 0, otherwise NOT x
        subl    %edx, %eax  # set %eax = x iff x >= 0 otherwise -x
        ret                 # return what is stored in %eax


/*====================================================================
 * int f3(int x)
 *
 * This function returns 1 if x is negative, and 0 if x is 0 or positive.
 * It does this by right shifting (arithmetic) by 31 bits, as above to get
 * the sign value, then it sets a value for the sign by comparing x against
 * this right shifted x. It will then set the %eax value to 1 if the sign
 * flag indicates negativity, or leave it as 0 if it is 0 or positive.
 * Then, it returns this value.
 */
.globl f3
f3:
        movl    %edi, %edx  # move x to %edx
        movl    %edx, %eax  # copy x to %eax
        sarl    $31, %eax   # arithmetic shift right 31 times
                            # (to get sign) store in %edx
        testl   %edx, %edx  # get sign, store it in a flag
        movl    $1, %edx    # set %edx to 1
        cmovg   %edx, %eax  # set %eax to value of sign flag
        ret                 # return %eax

