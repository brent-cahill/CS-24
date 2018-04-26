all: rlenc rldec test_rldec

CFLAGS = -g
ASFLAGS = -g


rlenc: rlenc.o
	$(CC) $(CFLAGS) $(LDFLAGS) rlenc.o -o rlenc

rldec: rldec.o rl_decode.o
	$(CC) $(CFLAGS) $(LDFLAGS) rldec.o rl_decode.o -o rldec

test_rldec: test_rldec.o rl_decode.o
	$(CC) $(CFLAGS) $(LDFLAGS) test_rldec.o rl_decode.o -o test_rldec

clean:
	rm -f *~ rlenc.o rldec.o test_rldec.o rl_decode.o \
	rlenc rldec test_rldec

.PHONY: all clean

