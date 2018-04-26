.globl gcd
gcd:

    movl %edi,%eax      # move x to %edx
    cqto                # convert quad to oct; sign extend
                        # %rax to %rdx
    idivl %esi          # divide x by y, store remainder in $edx
    cmpl $0,%edx        # if remainder is not 0,
    jne gcd_continue    # continue
    movl %esi,%eax      # if so, move y to %eax
    jmp gcd_return      # return y

gcd_continue:
    movl %esi,%edi      # move y to %edi (recursion, new x)
    movl %edx,%esi      # move remainder to %esi (recursion, new y)
    call gcd            # recursive call

gcd_return:
    ret
