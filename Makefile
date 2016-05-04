src = $(wildcard src/*.c)
obj = $(src:.c=.o)

CFLAGS = -g -Wall -Werror
LDFLAGS = -lncurses

zim: $(obj)
	    $(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
	clean:
	rm -f $(obj) zim
