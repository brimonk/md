CFLAGS=-g3 -Wall
all: md

md: md.c
	cc $(CFLAGS) -o $@ $^

clean: md # hah
	rm -f $^

