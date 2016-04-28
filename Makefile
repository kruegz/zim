all:
	gcc -g -Wall -Werror src/zim.c src/structures.c -o bin/zim -lncurses 
