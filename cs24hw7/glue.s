#============================================================================
# Keep a pointer to the main scheduler context.  This variable should be
# initialized to %rsp, which is done in the __sthread_start routine.
#
        .data
        .align 8
scheduler_context:      .quad   0

#============================================================================
# Integer variable for locking the scheduler
#
scheduler_lock:         .long   0


        .text

#============================================================================
# This function can be called from C to obtain the scheduler lock.
# It returns 1 if the lock is granted, 0 otherwise.
#
        .align 8
        .globl __sthread_lock
__sthread_lock:
        # In this code, we want to switch the values stored in the %eax
        # register and scheduler_lock. To do this, we will initially set the
        # value of %eax to be 1, as described in the c code in the set. Then
        # we will switch them, and %eax will hold the value of scheduler_lock.
        # Then we will return the boolean NOT of %eax. Here, we can use a
        # conditional set.

        movl    $1, %eax
        lock
        xchg	%eax, scheduler_lock
        testl	%eax, %eax 				# Get the 8-bit val for the Zero Flag.
        setz	%al 					# Set to the value of the zero flag.
        ret


#============================================================================
# This function can be called from C to release the scheduler lock.
#
        .align 8
        .globl __sthread_unlock
__sthread_unlock:
        # Simply release the lock by moving $0 into scheduler_lock
        movl	$0, scheduler_lock
        ret


#============================================================================
# __sthread_switch is the main entry point for the thread scheduler.
# It has three parts:
#
#    1. Save the context of the current thread on the stack.
#       The context includes all of the integer registers and RFLAGS.
#
#    2. Call __sthread_scheduler (the C scheduler function), passing the
#       context as an argument.  The scheduler stack *must* be restored by
#       setting %rsp to the scheduler_context before __sthread_scheduler is
#       called.
#
#    3. __sthread_scheduler will return the context of a new thread.
#       Restore the context, and return to the thread.
#
        .align 8
        .globl __sthread_switch
__sthread_switch:

        # Save the process state onto its stack
        pushq   %rax
        pushq   %rbx
        pushq   %rcx
        pushq   %rdx
        pushq   %rsi
        pushq   %rdi
        pushq   %rbp

        pushq   %r8
        pushq   %r9
        pushq   %r10
        pushq   %r11
        pushq   %r12
        pushq   %r13
        pushq   %r14
        pushq   %r15

        pushf

        # Call the high-level scheduler with the current context as an argument
        movq    %rsp, %rdi
        movq    scheduler_context, %rsp
        call    __sthread_scheduler

        # The scheduler will return a context to start.
        # Restore the context to resume the thread.
__sthread_restore:
        # The returned context should become the new stack, so we can
        # restore the machine context off of the stack.
        movq    %rax, %rsp

        popf

        popq    %r15
        popq    %r14
        popq    %r13
        popq    %r12
        popq    %r11
        popq    %r10
        popq    %r9
        popq    %r8

        popq    %rbp
        popq    %rdi
        popq    %rsi
        popq    %rdx
        popq    %rcx
        popq    %rbx
        popq    %rax

        ret


#============================================================================
# Initialize a process context, given:
#    1. the stack for the process
#    2. the function to start
#    3. its argument
# The context should be consistent with that saved in the __sthread_switch
# routine.
#
# A pointer to the newly initialized context is returned to the caller.
# (This is the thread's stack-pointer after its context has been set up.)
#
# This function is described in more detail in sthread.c.
#
#
        .align 8
        .globl __sthread_initialize_context
__sthread_initialize_context:
        # %rdi = stackp
        # %rsi = f
        # %rdx = arg

        # Set up return value in %rax.  This is the incoming stack pointer,
        # minus 144 bytes for the new thread context details and return
        # address.
        movq    %rdi, %rax
        subq    $144, %rax

        movq    $__sthread_finish, 136(%rax)   # When f returns, go here

        movq    %rsi, 128(%rax)    # Return address
        movq    $0,   120(%rax)    # rax = 0
        movq    $0,   112(%rax)    # rbx = 0
        movq    $0,   104(%rax)    # rcx = 0
        movq    $0,    96(%rax)    # rdx = 0
        movq    $0,    88(%rax)    # rsi = 0
        movq    %rdx,  80(%rax)    # rdi = arg
        movq    $0,    72(%rax)    # rbp = 0
        movq    $0,    64(%rax)    # r8  = 0
        movq    $0,    56(%rax)    # r9  = 0
        movq    $0,    48(%rax)    # r10 = 0
        movq    $0,    40(%rax)    # r11 = 0
        movq    $0,    32(%rax)    # r12 = 0
        movq    $0,    24(%rax)    # r13 = 0
        movq    $0,    16(%rax)    # r14 = 0
        movq    $0,     8(%rax)    # r15 = 0
        movq    $0,      (%rax)    # rflags = 0

        ret


#============================================================================
# The start routine initializes the scheduler_context variable, and calls
# the __sthread_scheduler with a NULL context.
#
# The scheduler will return a context to resume.
#
        .align 8
        .globl __sthread_start
__sthread_start:
        # Remember the context
        movq    %rsp, scheduler_context

        # Call the scheduler with no context
        movl    $0, %edi  # Also clears upper 4 bytes of %rdi
        call    __sthread_scheduler

        # Restore the context returned by the scheduler
        jmp     __sthread_restore

