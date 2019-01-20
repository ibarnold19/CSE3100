CC = cc --std=c99

all: shell

shell: shellp.c shellp.h
	$(CC) -g $< -o $@

clean:
	rm -rf *.o shell *~
