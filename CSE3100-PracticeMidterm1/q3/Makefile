CC     = gcc
CFLAGS = -g -w -std=c99

all: length

length: length.o
	$(CC) $^ -o $@

%.o : %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf length *.o *.swp *~
