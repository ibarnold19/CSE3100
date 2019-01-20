CC     = gcc
CFLAGS = -g -w -std=c99

all: binomial

binomial: binomial.o
	$(CC) $^ -o $@

%.o : %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf binomial *.o *.swp *~
