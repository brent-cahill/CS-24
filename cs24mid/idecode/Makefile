SOURCES=bus.c branching_program_counter.c instruction_store.c \
	branching_decode.c register_file.c alu.c branching_control.c \
	branch_unit.c branching_processor.c 

OBJECTS=$(SOURCES:.c=.o)
EXE=branching_run convert
CFLAGS = -g -DBRANCHING

all: $(EXE)

clean:
	rm -f *.o $(OBJECTS) $(EXE) *~

branching_program_counter.o:	branching_program_counter.c branching_program_counter.h instruction.h bus.h
instruction_store.o:	instruction_store.c instruction_store.h bus.h
branching_decode.o:	branching_decode.c branching_decode.h instruction.h bus.h
register_file.o:	register_file.c register_file.h instruction.h bus.h
alu.o:	alu.c alu.h instruction.h bus.h
branching_control.o:	branching_control.c alu.h register_file.h branching_decode.h instruction_store.h branching_program_counter.h
branch_unit.o:	branch_unit.c instruction.h bus.h
branching_processor.o:	branching_processor.c alu.h register_file.h branching_decode.h instruction_store.h branching_program_counter.h branching_control.h


branching_run:	bus.o branching_program_counter.o instruction_store.o \
	  branching_decode.o register_file.o branch_unit.o \
	  alu.o branching_control.o branching_processor.o run.o 
	gcc -o branching_run bus.o branching_program_counter.o instruction_store.o \
	  branching_decode.o branch_unit.o register_file.o alu.o \
	  branching_control.o branching_processor.o run.o

convert: convert.o
	gcc -o convert convert.o


.PHONY = clean all

