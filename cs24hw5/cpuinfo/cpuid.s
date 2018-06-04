.text


#=============================================================================
# unsigned int cpuid_0(char *vendor_string, unsigned int *max_ext_value)
#
#     Invokes the CPUID instruction with %eax set to 0, to retrieve the CPU's
#     vendor-string and the maximum value that can be used with CPUID.
#
# Arguments:
#     vendor_string (rdi) A pointer to where the vendor-string should be
#         stored.  This must point to a region of memory of at least 13 bytes.
#
#     max_ext_value (rsi) A pointer to where the maximum input value for
#         extended function CPUID information should be stored.  This must
#         point to a region of memory of at least 4 bytes.
#
# Returns:
#     The maximum value that can be used with the CPUID instruction.
#
.globl _cpuid_0
.globl cpuid_0
_cpuid_0:
cpuid_0:
    # Calle-save registers
    pushq %rbx

    # Get the maximum input value for basic CPUID information

    xorl %eax, %eax
    cpuid

    # eax will contain the max value that can be passed to cpuid,
    # so we'll return that value.  The other registers contain the
    # vendor string; store that into the location pointed to by
    # the argument.

    pushq %rax               # Save this value for later

    movl %ebx, (%rdi)
    movl %edx, 4(%rdi)
    movl %ecx, 8(%rdi)
    movb $0, 12(%rdi)

    # Get the maximum input value for extended CPUID information
    # eax will contain the max value for getting extended function information

    movl $0x80000000, %eax
    cpuid
    movl %eax, (%rsi)

    # Restore the max CPUID value for basic information, to return it
    popq %rax

    # Callee-save registers
    popq %rbx

    ret


#=============================================================================
# void cpuid_4(unsigned int ecx, regs_t *regs)
#
#     Invokes the CPUID instruction with %eax = 4, and %ecx set to the
#     specified value.  The results are stored into regs.
#
.globl _cpuid_4
.globl cpuid_4
_cpuid_4:
cpuid_4:
    pushq %rbx

    # Invoke CPUID with eax = 4 and ecx set to the specified value.
    movl $4, %eax
    movl %edi, %ecx
    cpuid

    # Store the register contents for the caller to use.
    movl %eax,   (%rsi)
    movl %ebx,  4(%rsi)
    movl %ecx,  8(%rsi)
    movl %edx, 12(%rsi)

    popq %rbx
    ret


#=============================================================================
# void cpuid_n(unsigned int eax, regs_t *regs)
#
#     Invokes the CPUID instruction with the specified value of %eax, and
#     stores the results into regs.
#
.globl _cpuid_n
.globl cpuid_n
_cpuid_n:
cpuid_n:
    pushq %rbx

    # Invoke CPUID with the specified value for %eax
    movl  %edi, %eax
    cpuid

    # Store the results into the target location
    movl %eax,   (%rsi)
    movl %ebx,  4(%rsi)
    movl %ecx,  8(%rsi)
    movl %edx, 12(%rsi)

    popq %rbx
    ret

