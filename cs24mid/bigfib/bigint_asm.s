# TODO:  Implement __add_bigint

.globl __add_bigint

__add_bigint:
	movq $1, %rax					# initialize return value
	test %dl, %dl					# Check if %dl == 0
	jz one_case						# If it is, go to the return one function

	clc										# Clear carry flag
	
	movq %rdx, %r8 				# put size variable into %r8
	movq %rdi, %r9				# a value now in %r9
	movq %rsi, %r10				# b value in %r10

	loop:
		movq (%r9), %r11		# "Dereference" %r8 into %r11, CALLER save
		adcq %r11, (%r10)		# add-carry the a and b vals.
		leaq 8(%r9), %r9		# load the next element in a
		leaq 8(%r10), %r10	# load the next element in b
		decq %r8						# decrement actual size variable
		jnz loop

	jc zero_case

	one_case:
		ret

	zero_case:
		movq $0, %rax
		ret
