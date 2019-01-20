CC=cc
CFLAGS=-g -std=c99 -lm

# -g 		add debug information
# -std=c99  	use the '99 version of the C standard
# -lm   	link with the math library (for functions like abs, ...)

all: collatz fibhw0

collatz: collatz.c
	-$(CC) $(CFLAGS) $< -o $@

fibhw0: fibhw0.c
	-$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o *~ collatz fibhw0


