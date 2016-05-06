# Use gcc compiler
CC = gcc

# -g : Include debugging information
# -Wall, -Werror : Show all errors and warnings
# -O2 : Do 2nd level optimization
# -v : Verbose, show all information
CFLAGS = -g -Wall -Werror -O2 -v

# Object files to build to prevent unnecessary compilation
OBJECTS = src/zim.o src/structures.o 

# Linker flags
LDFLAGS = -lncurses

# Default target that gets called by make
default : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -o bin/zim

# Make object files
%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean object files and binaries
clean:
	rm src/*.o
	rm -rf bin/*
