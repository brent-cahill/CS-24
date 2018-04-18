CC = gcc
CFLAGS = -Wall -Werror

all: factmain 

# perhaps use this as you develop further parts 
# of the assignment
#all: factmain gcdmain 

factmain.o:	fact.h factmain.c
fact.o:	fact.s

gcdmain.o:	gcd.h gcdmain.c
gcd.o:	gcd.s


factmain:	fact.o factmain.o
	$(CC) $(CFLAGS) -o factmain fact.o factmain.o

gcdmain:	gcd.o gcdmain.o
	$(CC) $(CFLAGS) -o gcdmain gcd.o gcdmain.o


clean:
	rm -f *.o factmain gcdmain


.PHONY: all clean

