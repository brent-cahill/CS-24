CC = gcc
CFLAGS = -Wall -Werror -g -O0


all: greeting


greeting: greeting.o output.o
	$(CC) $(CFLAGS) greeting.o output.o -o greeting $(LDFLAGS)


clean:
	rm -f *.o greeting

.PHONY: clean all

