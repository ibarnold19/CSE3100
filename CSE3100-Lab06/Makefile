CC=gcc
CFLAGS=-g  

all: md5

md5: md5.o
	$(CC) $< -o $@
	rm $<
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
clean:
	rm -rf *.o md5
