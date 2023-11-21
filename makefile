CC=gcc
CFLAGS=-std=c11
DEPS = defs.h
OBJ = generator.o syntax.o lexical.o semantic.o list.o stack.o symtable.o main.o defs.o

build: comp clean


%.o: %.c $(DEPS)
	$(CC) -c $^ $(CFLAGS)

comp: $(OBJ)
	$(CC) -o $@ *.o $(CFLAGS)

clean:
	rm -f *.o *.gch
