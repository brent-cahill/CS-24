#
# Compiler configuration
#

CFLAGS = -Wall -Werror -g
ASFLAGS = -g

# Object files:
OFILES = glue.o sthread.o queue.o timer.o semaphore.o bounded_buffer.o


#
# How to build the program
#

all: fibtest testlock


fibtest: fibtest.o $(OFILES)
	$(CC) $(CFLAGS) -o $@ $^

testlock: testlock.o $(OFILES)
	$(CC) $(CFLAGS) -o $@ $^


clean:
	rm -f *.o *~ fibtest testlock


.PHONY: all clean


#
# Dependencies
#
timer.o: timer.h glue.h
sthread.o: sthread.h glue.h queue.h
queue.o: queue.h sthread.h
bounded_buffer.o: bounded_buffer.h sthread.h glue.h
fibtest.o: sthread.h bounded_buffer.h
testlock.o: glue.h

