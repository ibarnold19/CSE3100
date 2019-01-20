CC=gcc
CFLAGS=-g

all: sumItFor sumItWhile pow

sumItFor : sumItFor.c
	$(CC) $< -o $@

sumItWhile: sumItWhile.c
	$(CC) $< -o $@

pow: pow.c
	$(CC) $< -o $@

%.o : $.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o sumItFor sumItWhile pow *~
