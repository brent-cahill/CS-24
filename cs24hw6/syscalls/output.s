# Question 1:
#  You simply need to trap into the kernel to invoke the write() system call to
# send the data to stdout. The write() call works as follows:
# ssize_t write(int fd, const void *buf, size_t count)
#
# fd:       "File descriptor"
# buf:      Address of the data buffer to write out
# count:    number of bytes to write
# ret:      number of bytes written
#
# Both count and ret are 64 bit integers.
#
# We know that the system call number will be contained in %rax, while the
# arguments to the call will be stored in:
# %rdi:     File descriptor
# %rsi:     Address
# %rdx:     Number of bytes
# as usual.
#
# It is important to note that the number of the write system call is 1, so we
# initially move this to 1 (See comments below).

.globl output
output:
    movq $1, %rax       # start with the write syscall
    pushq %rsi          # this is the size of buf
    movq %rdi, %rsi     # now we put the address in %rsi
    movq $1, %rdi       # file descriptor into %rdi
    popq %rdx           # size into %rdx
    syscall             # make the actual syscall
    ret                 # return call
