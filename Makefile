CC = gcc
DEFS = -D_BSD_SOURCE -D_DEFAULT_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)


.PHONY: all clean

all: myexpand

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

myexpand: myexpand.o
	$(CC) $(CFLAGS) -o myexpand myexpand.o

myexpand.o: myexpand.c myexpand.h
	$(CC) $(CFLAGS) -c -o myexpand.o myexpand.c

clean:
	rm -rf *.o 
	rm myexpand