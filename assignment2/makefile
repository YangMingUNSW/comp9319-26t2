CC = gcc
CFLAGS = -O2 -Wall -std=c11

all: bwtsearch

bwtsearch: bwtsearch.o bwt.o
	$(CC) $(CFLAGS) -o bwtsearch bwtsearch.o bwt.o

bwtsearch.o: bwtsearch.c bwt.h
	$(CC) $(CFLAGS) -c bwtsearch.c

bwt.o: bwt.c bwt.h
	$(CC) $(CFLAGS) -c bwt.c

clean:
	rm -f bwtsearch *.o
