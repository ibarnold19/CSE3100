CC=cc
CFLAGS=-g -std=c99
LFLAGS=-lm

# -g 		add debug information
# -std=c99  	use the '99 version of the C standard
# -lm   	link with the math library (for functions like abs, ...)

all: poly1 poly2 poly3 superp

poly1: poly1.c
	-$(CC) $(CFLAGS) $< -o $@

poly2: poly2.c reader.c 
	-$(CC) $(LFLAGS) $? -o $@

poly3: poly3.c reader.c
	-$(CC) $(LFLAGS) $? -o $@

superp: superPermutation.c
	-$(CC) $(CFLAGS) $? -o $@

%.o : %.c
	-$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o *~ poly1 poly2 poly3 superp



