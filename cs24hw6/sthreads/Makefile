#
# Commands and arguments
#

CC = gcc
RM = rm

CFLAGS = -Wall -g -std=c99 -pedantic
ASFLAGS = -g


all: test test_arg test_ret


# The simple test program
test: sthread.o queue.o glue.o test.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

test_arg: sthread.o queue.o glue.o test_arg.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

test_ret: sthread.o queue.o glue.o test_ret.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^


# pseudo-target to clean up
clean:
	$(RM) -f *.o core* *~ test


.PHONY: all clean


# Dependencies
sthread.c: sthread.h queue.h
queue.c: queue.h sthread.h
test.c: sthread.h
test_arg.c: sthread.h
test_ret.c: sthread.h

