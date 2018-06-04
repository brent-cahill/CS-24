CFLAGS = -g -O0


shapeinfo : shapes.o shapeinfo.o
	gcc $(LDFLAGS) -o $@ $^

# dependencies on header files
shapes.c : shapes.h
shapeinfo.c : shapes.h

clean :
	rm -f *.o shapeinfo shapeinfo.exe *~


.PHONY : clean

