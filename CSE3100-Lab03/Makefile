CC     = gcc
CFLAGS = -g -w -std=c99

all: caesar histogram alpha

caesar: caesar.c
	$(CC) $< -o $@

histogram: histogram.c
	$(CC) $< -o $@

alpha: alpha.c
	$(CC) $< -o $@

%.o : %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf caesar histogram alpha *.o *.swp *~
