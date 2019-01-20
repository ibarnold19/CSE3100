CC     = gcc
CFLAGS = -g -w -std=c99

all: stack

stack: stack.o
	$(CC) $^ -o $@

%.o : %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf stack *.o *.swp *~
