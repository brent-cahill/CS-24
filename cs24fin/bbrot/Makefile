OBJS=bbrot.o complex.o main.o

CFLAGS=-Wall -O2
LDFLAGS=-lpthread

all: bbrot

bbrot: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o bbrot $(LDFLAGS)

clean:
	rm -f $(OBJS) *~ bbrot

.PHONY: all clean

