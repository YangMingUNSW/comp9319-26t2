CC = gcc
CFLAGS = -O2 -Wall -std=c11

all: bwtsearch bwtdecode

bwtsearch: bwtsearch.o bwt.o
	$(CC) $(CFLAGS) -o bwtsearch bwtsearch.o bwt.o

bwtdecode: bwtdecode.o bwt.o
	$(CC) $(CFLAGS) -o bwtdecode bwtdecode.o bwt.o

bwtsearch.o: bwtsearch.c bwt.h
	$(CC) $(CFLAGS) -c bwtsearch.c

bwtdecode.o: bwtdecode.c bwt.h
	$(CC) $(CFLAGS) -c bwtdecode.c

bwt.o: bwt.c bwt.h
	$(CC) $(CFLAGS) -c bwt.c

clean:
	rm -f bwtsearch bwtdecode *.o
