#=============================================================================
# The fact(n) function recursively computes n! (n factorial).
#
.globl fact
fact:
        orl     $0, %edi        # Sets zero flag if arg equals zero
        jnz     fact_continue   # Compute factorial if nonzero
        movl    $1, %eax        # Otherwise return 1
        jmp     fact_return

fact_continue:
        pushq   %rdi            # Save our argument n before recursive call
        decl    %edi            # Compute n-1 for arg in recursive call
        call    fact            # Make recursive call; answer will be in eax

fact_resume:
        popq    %rdi            # Restore our argument n into rdi

        # fact(n-1) is in %eax; multiply %edi (n) into that for our answer!
        imull   %edi, %eax

fact_return:
        ret     # All done

