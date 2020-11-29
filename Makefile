CC=gcc
CFLAGS=-g
OBJ=kahn
FILES=vector.c graph.c file.c


# Compile without optimizations
make:
	$(CC) -o $(OBJ).o $(CFLAGS) $(OBJ).c $(FILES)

# Compile using maximum optimizations
3:
	$(CC) -o $(OBJ).o $(CFLAGS) $(OBJ).c $(FILES) -O3

# Clean the source files
clean:
	rm -f *.o

# Lint based on the included configuration using Clang
lint:
	find . -regex '.*\.\(c\|h\)' -exec clang-format-9 -style=file -i {} \;