CC     = gcc
CFLAGS = -g -w -std=c99

all: reverse

reverse: string.o
	$(CC) $^ -o $@

%.o : %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf reverse *.o *.swp *~
