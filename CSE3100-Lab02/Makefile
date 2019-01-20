CC     = gcc
CFLAGS = -g -w -std=c99

all: sumItFor quad factorial

sumItFor: sumItFor.c
	$(CC) $< -o $@

quad: quad.c
	$(CC) $< -o $@ -lm

factorial: factorial.c
	$(CC) $< -o $@ -lm

%.o : %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf sumItFor quad factorial *.o *.swp *~
