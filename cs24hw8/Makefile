CC = gcc
CFLAGS = -Wall -Werror -g -O0

VMEM_OBJS = virtualmem.o vmalloc.o matrix.o test_matrix.o

# So that the binary programs can be listed in fewer places.
# You will want to add to this variable as you implement various policies.
BINARIES = test_matrix


all: $(BINARIES)

# Compile this file with optimizations so that it accesses memory in
# reasonable ways.
matrix.o: CFLAGS += -O2

test_matrix: $(VMEM_OBJS) vmpolicy_random.o
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)


clean:
	rm -f *.o *~ $(BINARIES)


.PHONY: all clean

