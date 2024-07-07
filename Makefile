CC=gcc -std=c11
CFLAGS=-Wall -Wextra -pedantic -ggdb -O2
LIBS=-lm

all: bivi

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)
