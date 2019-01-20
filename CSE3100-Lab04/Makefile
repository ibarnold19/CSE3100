CC     = gcc
CFLAGS = -g -w -std=c99

all: squares strings

squares : squares.c
	$(CC) $< -o $@

strings : strings.c
	$(CC) $< -o $@

%.o : %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf squares strings *.o *.swp *~
