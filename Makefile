#src = $(wildcard src/*.c)
#obj = $(src:.c=.o)
#CC = gcc

#CFLAGS = -g -Wall -Werror
#LDFLAGS = -lncurses

#zim: $(obj)
		#$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

#.PHONY: clean
	#clean:
	#rm -f $(obj) zim

all:
	gcc -g -Wall -Werror src/zim.c src/structures.c -lncurses -o bin/zim
