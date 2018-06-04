ASFLAGS=-g
CFLAGS=-g -O0

all : bigfib

bigfib : bigfib.o bigint.o str.o bigint_asm.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean :
	rm -f *.o *~ bigfib

.PHONY : all clean

