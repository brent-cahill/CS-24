.globl my_setjmp

# This function stores the current execution state, into a passed "jump
# buffer"
#
# Stores the callee-save registers, the stack pointer, and the caller-return
# address all in the buffer (buf), then returns 0
#
# Important to note:
#	Callee-save registers:
#		%rbp,
#		%rbx,
#		%r12,
#		%r13,
#		%r14,
#		%r15.
#	Stack Pointer:
#		%rsp.
#	Caller-return address:
#		(%rsp).

my_setjmp:
	movq %rbp, (%rdi)		# Store val at %rdp with 0 offset in buf
	movq %rbx, 8(%rdi)		# Store val at %rbx with 8 offset in buf
	movq %r12, 16(%rdi)		# Store val at %r12 with 16 offset in buf
	movq %r13, 24(%rdi)		# Store val at %r13 with 24 offset in buf
	movq %r14, 32(%rdi)		# Store val at %r14 with 32 offset in buf
	movq %r15, 40(%rdi)		# Store val at %r15 with 40 offset in buf
	movq %rsp, 48(%rdi)		# Store pointer to stack with 48 offset in buf
	movq (%rsp), %rdx		# Store return address in %rdx
	movq %rdx, 56(%rdi)		# Then store rdx with 56 offset in buf
	movl $0, %eax			# Prepare to return value stored in %eax (0)
	ret

.globl my_longjmp

# This function restores the execution state saved in buf. This makes it
# appear as though it has returned everything from setjmp a second time,
# but this time, a nonzero value is returned. If the passed return value
# is nonzero, it will return that. Otherwise, it will return 1.
#
# Important to note:
#	Return value stored in %esi.
#	Everything else is the same.

my_longjmp:
	movq (%rdi), %rbp		# Move val at 0 offset to %rbp
	movq 8(%rdi), %rbx		# Move val at 8 offset to %rbx
	movq 16(%rdi), %r12		# Move val at 16 offset to %r12
	movq 24(%rdi), %r13		# Move val at 24 offset to %r13
	movq 32(%rdi), %r14		# Move val at 32 offset to %r14
	movq 40(%rdi), %r15		# Move val at 40 offset to %r15
	movq 48(%rdi), %rsp		# Move pointer to stack at 48 offset to %rsp
	movq 56(%rdi), %rdx		# Move return value at 56 offset back to %rdx
	movq %rdx, (%rsp)		# Move val at rdx back to %rsp
	movl $1, %eax			# Initialize %eax to 1
	testl %esi, %esi		# Test the returned value
	cmovnz %esi, %eax		# Move the passed val to eax if the ZF is not set
	ret
	