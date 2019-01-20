CC=gcc
CFLAGS=-g

all : hello

hello : hello.o
	$(CC) $< -o $@

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o hello *~
